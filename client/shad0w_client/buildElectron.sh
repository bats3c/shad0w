electron-packager . --platform=linux --overwrite --prune --ignore="(src|public|buildElectron.sh|.gitignore|.git|node_modules|logo.icns|logo.ico)" --icon="logo.png"
electron-packager . --platform=darwin --overwrite --prune --ignore="(src|public|buildElectron.sh|.gitignore|.git|node_modules|logo.png|logo.ico)" --icon="logo.icns"
#electron-packager . --platform=win32 --arch=x64 --overwrite --prune --ignore="(src|public|buildElectron.sh|.gitignore|.git|node_modules|logo.png|logo.icns)" --icon="logo.ico"
# Commented out until I can get a 64 bit wine for OSX working
