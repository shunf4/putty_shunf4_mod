#!/bin/sh

( BB_GLOBBING=0 env -u BB_GLOBBING -u PROFILEREAD -u SHELL -u ENV -u PS1 -u HOME -u PATH -u COMSPEC D:/work/putty/Release/pterm.exe -o 'MsWinConsoleBehaviourOnStart=attach' -o 'MsWinConptyFlags=PSEUDOCONSOLE_INHERIT_CURSOR|PSEUDOCONSOLE_WIN32_INPUT_MODE' -o MsWinWaitBeforeConsoleBehaviourMsec=1000 -o MsWinWaitAfterConsoleBehaviourMsec=0 -e D:/cygwin64/bin/bash.exe -c "/bin/bash -x -i -l ; /bin/sleep 0.1 # NOT ME,,,," & sleep 0.03 )

 