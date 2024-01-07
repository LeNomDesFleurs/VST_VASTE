# Using font in cpp

- [x] importer recursive
- [x] L'utiliser dans un label
- [ ] la modifier dynamiquement avec un bouton
- [ ] la modifier dynamiquement avec un slider

## Convertir la font en binarydata file

[juce forum](https://forum.juce.com/t/how-to-create-a-binarydata-object/6672/21)
Compile the `juce/extras/binarybuilder` (let's try first using Cmake)
There is a CMakeLists at the root of the binarybuilder


### solution 1 (doesn't work so far)
create a CMakeLists.txt in `assets/`

```cmake
juce_add_binary_data(myBinaryResourceTargetName SOURCES
        Jura-Light.ttf ) 
```

Dans le cmake principal du projet

```cmake
target_link_libraries(vstname
    PRIVATE
        # GuiAppData            # If we'd created a binary data target, we'd link to it here
        myBinaryResourceTargetName #this is your binary target
        #juce::juce_gui_extra
        #juce::juce_audio_utils
        #juce::juce_dsp
       
    PUBLIC
        #juce::juce_recommended_config_flags
        #juce::juce_recommended_lto_flags
        #juce::juce_recommended_warning_flags)
```

### solution 2 (the chosen one)

Drop the file in the assets folder
edit the cmakelist in /src where there is the

```cmake
juce_add_binary_data(AssetsBinary
SOURCES 
${CMAKE_SOURCE_DIR}/assets/juce-logo.svg
${CMAKE_SOURCE_DIR}/assets/name-of-the-assets.ttf
)
```

## Importer une font

[juce forum](https://forum.juce.com/t/modern-custom-font-guide/20841/6)

déclarer dans le header:
utiliser `Typeface::createSystemTypefaceFor()`

```cpp
Typeface::Ptr getTypefaceForFont(const Font& f) override
    {
        static Typeface::Ptr myFont = Typeface::createSystemTypefaceFor(BinaryData::uglyFont_otf,
            BinaryData::uglyFont_otfSize);
        return myFont;
    }
```

```cpp
Typeface::Ptr tface = Typeface::createSystemTypefaceFor(BinaryData::MontserratRegular_ttf, BinaryData::MontserratRegular_ttfSize);
setDefaultSansSerifTypeface (tface); //permet de la définir en global
```
