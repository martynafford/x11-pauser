# X11 pauser

A simple tool, which may be run as daemon, to suspend and resume the process
associated with the currently active window via the pause key. This can be
useful when running on a laptop and you wish to prolong your battery life by
conveniently suspending applications while they're not in use.

## Use

Focus the relevant application and press the pause key to suspend the
application. Depending on your window manager (compositing or otherwise) a
window may stop redrawing itself while the process is suspended. Press pause
again, with the application focused, to resume.

## Caveats

Extended Window Manager Hints (EWMH) support is required to obtain the active
window. Support is checked at start-up and you will see "Active window not
supported" if support is lacking.

To map a process ID to a window, the tool reads `_NET_WM_PID`. Applications
that do not set this will not work with this tool.

The tool will not work if the pause key is already bound (XGrabKey) by another
application. The code is currently hard coded to use the pause key but this is
easily changed.

If you have issues with which display to use, note that the tool uses the value
of the DISPLAY environment variable.

Applications that do more complicated things regarding processes (e.g.,
Chromium) won't work as expected.

This tool exists as an educational exploration of X11 rather than an exemplar.

## Licence

<img align="right" src="https://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

Licensed under the [MIT Licence](https://opensource.org/licenses/MIT):

Copyright &copy; 2018 [Martyn Afford](https://mafford.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
