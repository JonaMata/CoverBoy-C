#include "../easywsclient/easywsclient.hpp"
#include "../easywsclient/easywsclient.cpp" // <-- include only if you don't want compile separately
#define cimg_display 0
#include "../CImg/CImg.h"
#include <cassert>
#include <boost/json.hpp>
#include <string>
#include <chrono>
#include <csignal>
#include "../rpi-rgb-led-matrix/include/led-matrix.h"

using namespace boost;
using namespace cimg_library;
using easywsclient::WebSocket;
using rgb_matrix::RGBMatrix;
using rgb_matrix::FrameCanvas;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
    interrupt_received = true;
}

RGBMatrix *matrix;
static WebSocket::pointer ws = nullptr;
bool authenticated = false;
int retries = 0;
std::string current_song;
std::chrono::steady_clock::time_point last_update = std::chrono::steady_clock::now();

std::string access_token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJkODliOWU4NzgyOWU0MjM2YjYxNDQ3ZTcwY2ZlYTdiOCIsImlhdCI6MTczODUxMzE3NCwiZXhwIjoyMDUzODczMTc0fQ.Jr6TD93QhfHSyrFPckuIyZUJldSb4tXOE1JrTbKKpAU";

void handle_message(const std::string & message)
{
    json::value mess = json::parse(message);
    if (mess.at("type") == "auth_required") {
        ws->send(json::serialize(json::object{{"type", "auth"}, {"access_token", access_token}}));
    } else if (message.starts_with(R"({"type":"auth_ok")")) {
        authenticated = true;
        ws->send(json::serialize(json::object{{"id", 1}, {"type", "subscribe_events"}, {"event_type", "state_changed"}}));
    } else {
        if (mess.kind() == json::kind::object) {
            if (mess.at("type").as_string() == "event") {
                if (mess.at("event").at("data").at("entity_id").as_string().starts_with("media_player.")) {
                    json::value player = mess.at("event").at("data").at("new_state");
                    if (player.at("state") == "playing") {
                        json::value attr = player.at("attributes");
                        if (attr.at("media_content_type") == "music") {
                            last_update = std::chrono::steady_clock::now();
                            if (current_song == attr.at("media_title").as_string()) {
                                return;
                            }
                            printf("Playing: %s - %s\n", attr.at("media_artist").as_string().c_str(), attr.at("media_title").as_string().c_str());
                            std::string cover_url = "http://sweetchili:8123";
                            if (attr.as_object().if_contains("entity_picture_local")) {
                                cover_url += attr.at("entity_picture_local").as_string().c_str();
                            } else if (attr.as_object().if_contains("entity_picture")) {
                                cover_url += attr.at("entity_picture").as_string().c_str();
                            }
                            printf("Cover: %s\n", cover_url.c_str());

                            try {
                                CImg<unsigned char> cover(cover_url.c_str());
                                if (cover.width() > cover.height()) {
                                    int start = (cover.width() - cover.height()) / 2;
                                    cover.crop(start, 0, start + cover.height() - 1, cover.height() - 1);
                                }
                                cover.resize(64, 64);
                                FrameCanvas *offscreen_canvas = matrix->CreateFrameCanvas();
                                for (int x = 0; x < 64; x++) {
                                    for (int y = 0; y < 64; y++) {
                                        offscreen_canvas->SetPixel(x, y, cover(x, y, 0, 0), cover(x, y, 0, 1), cover(x, y, 0, 2));
                                    }
                                }
                                matrix->SwapOnVSync(offscreen_canvas);
                                printf("Saved\n");
                            } catch (CImgIOException & e) {
                                printf("Error: %s\n", e.what());
                            }
                        }
                    }
                }
            }
        }
    }
}

void start_ws() {
    ws = WebSocket::from_url("ws://sweetchili:8123/api/websocket");
    assert(ws);
    while (ws->getReadyState() != WebSocket::CLOSED) {
        if (interrupt_received) {
            return;
        }
        if (std::chrono::duration_cast<std::chrono::minutes>(std::chrono::steady_clock::now() - last_update).count() > 10) {
            matrix->Clear();
        }
        ws->poll();
        ws->dispatch(handle_message);
    }
    delete ws;
}

int main(int argc, char *argv[])
{
    RGBMatrix::Options defaults;
    defaults.hardware_mapping = "regular";  // or e.g. "adafruit-hat"
    defaults.rows = 64;
    defaults.cols = 64;
    defaults.chain_length = 1;
    defaults.parallel = 1;
    defaults.show_refresh_rate = true;
    matrix = RGBMatrix::CreateFromFlags(&argc, &argv, &defaults);
    if (matrix == nullptr)
        return 1;
    matrix->SetBrightness(50);
    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);
    while (!interrupt_received && retries < 5) {
        if (!authenticated) {
            retries += 1;
        } else {
            retries = 0;
        }
        authenticated = false;
        start_ws();
    }
    matrix->Clear();
    delete matrix;
    return 0;
}