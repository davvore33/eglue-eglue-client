#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
std::string host = "127.0.0.1";
auto const port = "12345";

std::string text;

int main(int argc, char **argv) {
    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
                ("help,h", "Help screen")
                ("load", boost::program_options::value<std::string>(), "Load path")
                ("get", boost::program_options::value<std::string>(), "Get key")
                ("set", boost::program_options::value<std::vector<std::string>>()->multitoken(), "Set key value");

        boost::program_options::variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help")) {
            std::cerr << desc << '\n';
            exit(1);
        } else if (vm.count("load")) {
            text = "LOAD " + vm["load"].as<std::string>() + "\n";
        } else if (vm.count("get")) {
            text = "GET " + vm["get"].as<std::string>() + "\n";
        } else if (vm.count("set")) {
            std::vector<std::string> options = vm["set"].as<std::vector<std::string>>();
            text = "SET " + options[0] + " " + options[1] + "\n";
        } else {
            std::cerr << desc << '\n';
            exit(1);
        }
    }
    catch (const boost::program_options::error &e) {
        BOOST_LOG_TRIVIAL(error) << "Error " << e.what();
    }

    try {
        net::io_context ioc;
        tcp::resolver resolver{ioc};
        websocket::stream<tcp::socket> ws{ioc};
        auto const results = resolver.resolve(host, port);
        auto ep = net::connect(ws.next_layer(), results);
        host += ':' + std::to_string(ep.port());

        ws.set_option(websocket::stream_base::decorator(
                [](websocket::request_type &req) {
                    req.set(http::field::user_agent,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                            " websocket-client-coro");
                }));

        ws.handshake(host, "/");
        ws.write(net::buffer(std::string(text)));
        beast::flat_buffer buffer;
        ws.read(buffer);
        ws.close(websocket::close_code::normal);

        BOOST_LOG_TRIVIAL(info) << beast::make_printable(buffer.data());
    }
    catch (std::exception const &e) {
        BOOST_LOG_TRIVIAL(error) << "Error " << e.what();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


