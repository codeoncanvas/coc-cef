*Webviews in Cinder using Chrome Embedded Framework*  

![screenshot1.png](https://raw.githubusercontent.com/codeoncanvas/coc-cef/master/images/screenshot1.png)

This allows you to embed web elements in your Cinder app, including WebGL. Web browser behaviour should be mostly the same as Google Chrome.  
Warning: This project is rough around the edges. Currently only macOS with Xcode is supported. Windows support may follow soon.

Sample app code is provided in `samples/cocCefSample.cpp`, remember to change `cocCefSample::` to your app class name.


## Setup steps for macOS
- Clone repository

- Run `sh extract.sh` to extract the compressed Chrome Embedded Framework (built for macOS 64bit).

- Use TinderBox to add the coc-cef block.

- in `Target -> Build Phases`: add a New Run Script Phase,  
Shell: bin/sh
```
exec install_name_tool -change "@rpath/Frameworks/Chromium Embedded Framework.framework/Chromium Embedded Framework" "@rpath/Chromium Embedded Framework.framework/Chromium Embedded Framework" "${BUILT_PRODUCTS_DIR}/${EXECUTABLE_PATH}"
exit 1
```

![image1.png](https://raw.githubusercontent.com/codeoncanvas/coc-cef/master/images/image1.png)

- Duplicate target with same name but append " Helper" (must have a space).
Edit the build scheme name for consistency.

![image2.png](https://raw.githubusercontent.com/codeoncanvas/coc-cef/master/images/image2.png)
![image3.png](https://raw.githubusercontent.com/codeoncanvas/coc-cef/master/images/image3.png)

- Remove all source target memberships from Target Helper -> Build Phases -> Compile Sources
Add Blocks/CoC-CEF/src/process_helper_mac.cpp to the Helper target

![image4.png](https://raw.githubusercontent.com/codeoncanvas/coc-cef/master/images/image4.png)  

- On main target, under Build Phases -> Copy Files, add Chromium Embedded Framework.framework

![image5.png](https://raw.githubusercontent.com/codeoncanvas/coc-cef/master/images/image5.png)

- Build the Helper, Build the main target, and you are good to go!


## Known Issues
- Not currently using the most recent version of CEF
- App doesn't exit cleanly, see [here](http://www.magpcss.org/ceforum/viewtopic.php?f=6&t=11441&p=24037&hilit=AutoreleasePoolPage#p24037)
- Including built CEF framework in repo is not ideal

## Contributing
PRs or general tips & suggestions are welcome!

## Reference and Related Links
- [ofZach/ofxCef - OpenFrameworks CEF Plugin](https://github.com/ofZach/ofxCef)
- [CEF Project](https://bitbucket.org/chromiumembedded/cef-project)
- [WebGL in Cinder blog](http://www.thegrego.com/2016/05/16/webgl-in-cinder/)
