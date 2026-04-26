#include "putty.h"
#include "storage.h"
#include <fcntl.h>

extern void tryAttachConsoleAndResetCrtFds();

const unsigned cmdline_tooltype =
    TOOLTYPE_NONNETWORK |
    TOOLTYPE_NO_VERBOSE_OPTION;


static void show_help(void)
{
    const char *helptext =
        "pterm: PuTTY-style terminal for local command prompts\n"
        "\n"
        "Usage: pterm [OPTIONS] [-e COMMAND [ARGS...]]\n"
        "\n"
        "Options:\n"
        "  -h, --help, -?, /?         Show this help message\n"
        "  -load SESSION              Load a saved session by name\n"
        "  @SESSION                   Load a saved session (alternative syntax)\n"
        "  -o KEY=VALUE               Override a configuration setting\n"
        "  -sessionlog FILE           Log session output to FILE\n"
        "  -e COMMAND [ARGS...]       Run COMMAND instead of the default shell\n"
        "\n"
        "Common -o overrides:\n"
        "\n"
        "  Window size and position:\n"
        "    -o TermWidth=80          Terminal width in columns\n"
        "    -o TermHeight=24         Terminal height in rows\n"
        "    -o \"WinTitle=My Title\"   Set the initial window title\n"
        "    -o \"StartupWindowPos=@active:center\"\n"
        "                             Center on the monitor with mouse cursor\n"
        "    -o \"StartupWindowPos=@screen:100,200\"\n"
        "                             Position in virtual desktop coordinates\n"
        "    -o \"StartupWindowPos=0:0,0\"\n"
        "                             Top-left of the first monitor (by index)\n"
        "    -o \"StartupWindowPos=\\\\.\\DISPLAY2:center\"\n"
        "                             By monitor device name\n"
        "\n"
        "  StartupWindowPos format: <monitor>:<position>\n"
        "    <monitor> can be:\n"
        "      @active   = monitor containing the mouse cursor\n"
        "      @screen   = virtual desktop (all monitors combined)\n"
        "      <number>  = monitor index (0, 1, 2, ...)\n"
        "      <name>    = device name, e.g. \\\\.\\DISPLAY1\n"
        "    <position> can be:\n"
        "      x,y       = pixel coordinates relative to the monitor area\n"
        "      center    = center the window\n"
        "\n"
        "  Scrollback:\n"
        "    -o ScrollbackLines=2000  Number of scrollback lines\n"
        "    -o ScrollBar=true        Show/hide the scrollbar (true/false)\n"
        "\n"
        "  Font:\n"
        "    -o \"Font=Consolas,14\"    Font name and size (height in points)\n"
        "    -o \"Font=Courier New,12\" Font name with spaces must be quoted\n"
        "\n"
        "  Colours:\n"
        "    -o Colour0=255,255,255   Default Foreground (R,G,B)\n"
        "    -o Colour2=0,0,0         Default Background (R,G,B)\n"
        "    -o Colour1=255,255,255   Default Bold Foreground (R,G,B)\n"
        "\n"
        "  Copy & paste:\n"
        "    -o CtrlShiftCV=explicit  Enable Ctrl+Shift+C/V copy/paste\n"
        "                             Values: none, implicit, explicit,\n"
        "                                     custom:<clipboard-name>\n"
        "    -o MousePaste=implicit   Mouse paste mode\n"
        "    -o MouseAutocopy=true    Auto-copy selected text\n"
        "\n"
        "  Zoom (windowed: resize window; maximized/fullscreen: change cols/rows):\n"
        "    Ctrl+Scroll Up/Down      Zoom font size in/out\n"
        "    Ctrl+Shift++             Zoom in  (Ctrl+Shift+= on US keyboard)\n"
        "    Ctrl+Shift+-             Zoom out (Ctrl+Shift+_ on US keyboard)\n"
        "    Ctrl+0                   Reset font to default size\n"
        "\n"
        "Boolean values: true/false, yes/no, or 1/0.\n"
        "Integer values: decimal numbers.\n"
        "Enum integer values: integer or enum constant name (e.g. B_IND_FLASH).\n"
        "Font values: FontName[,Height[,Bold[,Charset]]]\n"
        "Colour values: R,G,B (three integers 0-255).\n"
        "\n"
        "  Bell / scrollback:\n"
        "    -o BeepInd=0             Bell indication (0=disabled, 1=flash,\n"
        "                             2=steady, or B_IND_DISABLED/B_IND_FLASH/\n"
        "                             B_IND_STEADY)\n"
        "    -o ScrollOnKey=true      Scroll to bottom on keypress\n"
        "    -o ScrollOnDisp=true     Scroll to bottom on new output\n"
        "    -o ScrollBarFullScreen=false\n"
        "                             Show scrollbar in fullscreen mode\n"
        "\n"
        "  Windows console behaviour (pterm-specific):\n"
        "    -o MsWinConsoleBehaviourOnStart=attach\n"
        "                             Console behaviour on startup:\n"
        "                               attach    = AttachConsole(parent) (default)\n"
        "                               alloc     = AllocConsole()\n"
        "                               allocHide = (not implemented)\n"
        "                               none      = do not attach/allocate\n"
        "\n"
        "  ConPTY flags (pterm-specific):\n"
        "    -o \"MsWinConptyFlags=PSEUDOCONSOLE_INHERIT_CURSOR|PSEUDOCONSOLE_WIN32_INPUT_MODE\"\n"
        "                             Flags for CreatePseudoConsole.\n"
        "                             Use flag names with | or a hex/dec integer.\n"
        "                             Default (empty): all flags (0x7).\n"
        "                             Available flags:\n"
        "                               PSEUDOCONSOLE_INHERIT_CURSOR   (0x1)\n"
        "                               PSEUDOCONSOLE_RESIZE_QUIRK     (0x2)\n"
        "                               PSEUDOCONSOLE_WIN32_INPUT_MODE (0x4)\n"
        "  -o MsWinWaitBeforeConsoleBehaviourMsec=2000\n"
        "  -o MsWinWaitAfterConsoleBehaviourMsec=2000\n"
        "\n"
        "For a complete list of configuration keys and their meanings,\n"
        "see the Windows registry under:\n"
        "  HKEY_CURRENT_USER\\Software\\SimonTatham\\PuTTY\\Sessions\\\n"
        "or configure settings in PuTTY.exe and use -load to reuse them.\n"
        "\n"
        "Examples:\n"
        "  pterm -o TermWidth=120 -o TermHeight=36 -o ScrollbackLines=10000\n"
        "  pterm -o \"Font=Consolas,14\" -o CtrlShiftCV=explicit\n"
        "  pterm -o WinTitle=Server -o \"StartupWindowPos=@active:center\"\n"
        "  pterm -o Colour0=255,255,255 -o Colour2=30,30,30  (light on dark)\n"
        "  pterm -load \"Default Settings\" -o TermWidth=100 -e cmd\n"
        "  pterm -o MsWinConsoleBehaviourOnStart=none \\\n"
        "        -o MsWinConptyFlags=0x5 -e wsl\n";

    {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        HANDLE hErr = GetStdHandle(STD_ERROR_HANDLE);
        HANDLE hIn  = GetStdHandle(STD_INPUT_HANDLE);
        DWORD modeDummy;

        BOOL stdoutTouched, stderrTouched, stdInTouched;

        if (hOut == NULL) {
            stdoutTouched = FALSE;
        } else if (hOut == INVALID_HANDLE_VALUE) {
            stdoutTouched = FALSE; // ???
        } else if (GetConsoleMode(hOut, &modeDummy)) {
            stdoutTouched = TRUE;
        } else {
            stdoutTouched = TRUE;
        }

        if (hErr == NULL) {
            stderrTouched = FALSE;
        } else if (hErr == INVALID_HANDLE_VALUE) {
            stderrTouched = FALSE; // ???
        } else if (GetConsoleMode(hErr, &modeDummy)) {
            stderrTouched = TRUE;
        } else {
            stderrTouched = TRUE;
        }

        if (hIn == NULL) {
            stdInTouched = FALSE;
        } else if (hIn == INVALID_HANDLE_VALUE) {
            stdInTouched = FALSE; // ???
        } else if (GetConsoleMode(hIn, &modeDummy)) {
            stdInTouched = TRUE;
        } else {
            stdInTouched = TRUE;
        }

        if (!stdoutTouched && !stdInTouched && !stderrTouched) {
            // Attach console from a GUI app, so printing to stdout
            // and stderr works if the app is invoked in a console.
            tryAttachConsoleAndResetCrtFds();
        }
    }
    fprintf(stdout, "%s\n", helptext);
    fflush(stdout);

    /* Also show as a dialog (the primary UI for a GUI app) */
    char *title = dupprintf("%s Usage", appname);
    MessageBoxA(NULL, helptext, title, MB_ICONINFORMATION | MB_OK);
    sfree(title);

    exit(0);
}

void gui_term_process_cmdline(Conf *conf, char *cmdline)
{
    do_defaults(NULL, conf);
    conf_set_str(conf, CONF_remote_cmd, "");

    cmdline = handle_restrict_acl_cmdline_prefix(cmdline);
    if (handle_special_sessionname_cmdline(cmdline, conf) ||
        handle_special_filemapping_cmdline(cmdline, conf))
        return;

    CmdlineArgList *arglist = cmdline_arg_list_from_GetCommandLineW();
    size_t arglistpos = 0;
    while (arglist->args[arglistpos]) {
        CmdlineArg *arg = arglist->args[arglistpos++];
        CmdlineArg *nextarg = arglist->args[arglistpos];
        const char *argstr = cmdline_arg_to_str(arg);
        int retd = cmdline_process_param(arg, nextarg, 1, conf);
        if (retd == -2) {
            cmdline_error("option \"%s\" requires an argument", argstr);
        } else if (retd == 2) {
            arglistpos++;              /* skip next argument */
        } else if (retd == 1) {
            continue;          /* nothing further needs doing */
        } else if (!strcmp(argstr, "-h") || !strcmp(argstr, "--help") ||
                   !strcmp(argstr, "-?") || !strcmp(argstr, "/?")) {
            show_help();
        } else if (!strcmp(argstr, "-e")) {
            if (nextarg) {
                /* The command to execute is taken to be the unparsed
                 * version of the whole remainder of the command line. */
                char *cmd = cmdline_arg_remainder_utf8(nextarg);
                conf_set_utf8(conf, CONF_remote_cmd, cmd);
                sfree(cmd);
                return;
            } else {
                cmdline_error("option \"%s\" requires an argument", argstr);
            }
        } else if (!strcmp(argstr, "-o")) {
            if (nextarg) {
                const char *optstr = cmdline_arg_to_str(nextarg);
                const char *eq = strchr(optstr, '=');
                if (!eq)
                    cmdline_error("syntax: -o Key=Value");
                char *kw = dupprintf("%.*s", (int)(eq - optstr), optstr);
                if (!conf_apply_override(conf, kw, eq + 1))
                    cmdline_error("unrecognised or unsupported option "
                                  "\"%s\"", kw);
                sfree(kw);
                arglistpos++;
            } else {
                cmdline_error("option \"-o\" requires an argument");
            }
        } else if (argstr[0] == '-') {
            cmdline_error("unrecognised option \"%s\"", argstr);
        } else {
            cmdline_error("unexpected non-option argument \"%s\"", argstr);
        }
    }

    cmdline_run_saved(conf);

    conf_set_int(conf, CONF_sharrow_type, SHARROW_BITMAP);
}

const struct BackendVtable *backend_vt_from_conf(Conf *conf)
{
    return &conpty_backend;
}

const wchar_t *get_app_user_model_id(void)
{
    return L"SimonTatham.Pterm.shunf4-mod";
}

void gui_terminal_ready(HWND hwnd, Seat *seat, Backend *backend)
{
}
