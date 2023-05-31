import ice.example;
#include <boost/asio/as_tuple.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <chrono>
#include <iostream>

using token = decltype(boost::asio::as_tuple(boost::asio::use_awaitable));

template <class Body, class Allocator>
boost::beast::http::message_generator handle_request(
  boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>>&& request)
{
  boost::beast::http::response<boost::beast::http::string_body> response{
    boost::beast::http::status::ok,
    request.version(),
  };
  response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
  response.set(boost::beast::http::field::content_type, "text/plain");
  response.keep_alive(request.keep_alive());
  response.body() = "Hello World!";
  response.prepare_payload();
  return response;
}

boost::asio::awaitable<void> session(boost::asio::ip::tcp::socket socket)
{
  using namespace std::chrono_literals;

  auto executor = co_await boost::asio::this_coro::executor;
  auto timer = token::as_default_on(boost::asio::steady_timer{ executor });

  boost::system::error_code ec;
  auto client = socket.remote_endpoint(ec);
  if (ec) {
    std::cerr << "open failed: " << ec << std::endl;
    co_return;
  }
  std::cout << "opened " << client << std::endl;

  boost::beast::flat_buffer buffer;
  boost::beast::http::request<boost::beast::http::string_body> request;
  auto stream = token::as_default_on(boost::beast::tcp_stream{ std::move(socket) });

  while (true) {
    stream.expires_after(std::chrono::seconds(30));
    if (const auto [ec, _] = co_await boost::beast::http::async_read(stream, buffer, request); ec) {
      std::cerr << "recv " << client << ": " << ec << std::endl;
      break;
    }

    std::cout << client << " request:\n" << request << std::endl;

    // auto message = handle_request(std::move(request));
    // const auto keep_alive = message.keep_alive();
    // if (const auto [ec, _] = co_await boost::beast::async_write(stream, std::move(message)); ec) {
    //   std::cerr << "send " << client << ": " << ec << std::endl;
    //   break;
    // }
    // std::cout << client << " response:\n" << response << std::endl;
    // if (!keep_alive) {
    //   break;
    // }
    
    auto keep_alive = request.keep_alive();

    boost::beast::http::response<boost::beast::http::buffer_body> response{
      boost::beast::http::status::ok,
      request.version(),
    };

    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::transfer_encoding, "chunked");
    response.set(boost::beast::http::field::content_type, "text/plain");
    response.keep_alive(keep_alive);
    response.version(11);

    response.body().data = nullptr;
    response.body().more = true;

    boost::beast::http::response_serializer<boost::beast::http::buffer_body> sr{ response };
    if (const auto [ec, _] = co_await boost::beast::http::async_write_header(stream, sr); ec) {
      std::cerr << "send header " << client << ": " << ec << std::endl;
      break;
    }

    while (true) {
      char data[2] = { 'A', '\n' };
      response.body().data = data;
      response.body().size = sizeof(data);
      response.body().more = true;

      const auto [ec, _] = co_await boost::beast::http::async_write(stream, sr);
      if (ec && ec != boost::beast::http::error::need_buffer) {
        std::cerr << "send data " << client << ": " << ec << std::endl;
        keep_alive = false;
        break;
      }

      timer.expires_from_now(1s);
      co_await timer.async_wait();
    }

    if (!keep_alive) {
      break;
    }
  }
  std::cout << "closed " << client << std::endl;
  stream.socket().close(ec);
  co_return;
}

boost::asio::awaitable<void> listen(boost::asio::ip::tcp::endpoint endpoint)
{
  auto executor = co_await boost::asio::this_coro::executor;
  auto acceptor = token::as_default_on(boost::asio::ip::tcp::acceptor{ executor });

  boost::system::error_code ec;
  acceptor.open(endpoint.protocol(), ec);
  if (ec) {
    std::cerr << "could not open acceptor: " << ec << std::endl;
    co_return;
  }

  acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if (ec) {
    std::cerr << "could not set reuse address option: " << ec << std::endl;
    co_return;
  }

  acceptor.bind(endpoint, ec);
  if (ec) {
    std::cerr << "could not bind to " << endpoint.address() << ": " << ec << std::endl;
    co_return;
  }

  acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
  if (ec) {
    std::cerr << "could not listen on port " << endpoint.port() << ": " << ec << std::endl;
    co_return;
  }

  while (true) {
    auto [ec, socket] = co_await acceptor.async_accept();
    if (ec) {
      std::cerr << "could not accept connection: " << ec << std::endl;
      continue;
    }
    boost::asio::co_spawn(executor, session(std::move(socket)), boost::asio::detached);
  }
  co_return;
}

int main(int argc, char* argv[])
{
  boost::asio::io_context context{ 1 };
  const auto address = boost::asio::ip::make_address("127.0.0.1");
  const boost::asio::ip::tcp::endpoint endpoint{ address, 8080 };
  boost::asio::co_spawn(context, listen(endpoint), boost::asio::detached);
  context.run();
}
