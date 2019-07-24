// Copyright (c) 2018 Martyn Afford
// Licensed under the MIT licence

#include <X11/Xlib.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace {

Display* display;
Window root;

template <typename T>
class x11_data {
public:
    unsigned char* properties = nullptr;
    unsigned long count = 0;

    x11_data()
    {}

    x11_data(unsigned char* props, unsigned long num)
        : properties{props}
        , count{num}
    {}

    x11_data(x11_data&& other)
        : x11_data{other.properties, other.count}
    {
        other.properties = nullptr;
    }

    ~x11_data()
    {
        if (properties != nullptr) {
            XFree(properties);
        }
    }

    x11_data(const x11_data&) = delete;
    x11_data&
    operator=(const x11_data&) = delete;
    x11_data&
    operator=(x11_data&&) = delete;

    const T*
    begin() const
    {
        return reinterpret_cast<T*>(properties);
    }

    const T*
    end() const
    {
        return begin() + count;
    }
};

Atom
make_atom(const char* x)
{
    return XInternAtom(display, x, False);
}

template <typename T>
x11_data<T>
get_window_properties(Atom atom, Window window = root)
{
    long offset = 0;
    long length = ~0l;
    Bool delete_property = False;
    Atom actual_type;
    int actual_format;
    unsigned long item_count;
    unsigned long bytes_remaining;
    unsigned char* properties;

    auto status = XGetWindowProperty(display, window, atom, offset, length,
                                     delete_property, AnyPropertyType,
                                     &actual_type, &actual_format, &item_count,
                                     &bytes_remaining, &properties);

    if (status != Success) {
        fprintf(stderr, "XGetWindowProperty: failed\n");
        return {};
    }

    return {properties, item_count};
}

bool
active_window_supported()
{
    auto active_window = make_atom("_NET_ACTIVE_WINDOW");

    for (Atom atom : get_window_properties<Atom>(make_atom("_NET_SUPPORTED"))) {
        if (atom == active_window) {
            return true;
        }
    }

    return false;
}

Window
get_active_window()
{
    auto x = get_window_properties<Window>(make_atom("_NET_ACTIVE_WINDOW"));
    return x.count ? *x.begin() : 0;
}

int
get_window_pid(Window w)
{
    auto x = get_window_properties<unsigned long>(make_atom("_NET_WM_PID"), w);
    return x.count ? *x.begin() : 0;
}

bool
is_process_stopped(int pid)
{
    constexpr auto length = 512;
    char buf[length];

    snprintf(buf, length, "/proc/%d/status", pid);
    auto fd = open(buf, O_RDONLY);

    if (fd < 0) {
        fprintf(stderr, "open: failed\n");
        return true;
    }

    if (read(fd, buf, length) <= 0) {
        fprintf(stderr, "read: failed\n");
        close(fd);
        return true;
    }

    close(fd);
    return strstr(buf, "\nState:\tT") != nullptr;
}

void
toggle_active_window()
{
    auto window = get_active_window();

    if (window == 0) {
        fprintf(stderr, "get_active_window: failed\n");
        return;
    }

    auto pid = get_window_pid(window);

    if (pid == 0) {
        fprintf(stderr, "get_window_pid: failed\n");
        return;
    }

    if (kill(pid, is_process_stopped(pid) ? SIGCONT : SIGSTOP) != 0) {
        fprintf(stderr, "kill: failed\n");
        return;
    }
}

} // namespace

int
main()
{
    display = XOpenDisplay(nullptr);

    if (display == nullptr) {
        fprintf(stderr, "Cannot open display\n");
        return EXIT_FAILURE;
    }

    atexit([]() { XCloseDisplay(display); });
    root = XDefaultRootWindow(display);

    if (!active_window_supported()) {
        fprintf(stderr, "Active window not supported\n");
        return EXIT_FAILURE;
    }

    auto pause_key_sym = 0xff13;
    auto key_code = XKeysymToKeycode(display, pause_key_sym);
    auto owner_events = False;
    auto pointer_mode = GrabModeAsync;
    auto keyboard_mode = GrabModeAsync;
    auto status = XGrabKey(display, key_code, AnyModifier, root, owner_events,
                           pointer_mode, keyboard_mode);

    if (status != 1) {
        fprintf(stderr, "XGrabKey: failed\n");
        return EXIT_FAILURE;
    }

    status = XSelectInput(display, root, KeyPressMask);

    if (status != 1) {
        fprintf(stderr, "XSelectInput: failed\n");
        return EXIT_FAILURE;
    }

    while (true) {
        XEvent event;
        XNextEvent(display, &event);

        if (event.type == KeyPress) {
            toggle_active_window();
        }
    }

    return EXIT_SUCCESS;
}
