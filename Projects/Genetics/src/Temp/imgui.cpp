//ImGui library v1.47 WIP
//Main code & documentation

//See ImGui::ShowTestWindow() in imgui_demo.cpp for demo code.
//Read 'Programmer guide' below for notes on how to setup ImGui in your codebase.
//Get latest version at https://github.com/ocornut/imgui
//Releases change-log at https://github.com/ocornut/imgui/releases
//Developed by Omar Cornut and every direct or indirect contributors to the GitHub.
//This library is free but I need your support to sustain development and maintenance.
//If you work for a company, please consider financial support, e.g: https://www.patreon.com/imgui

/*

 Index
 - MISSION STATEMENT
 - END-USER GUIDE
 - PROGRAMMER GUIDE (read me!)
 - API BREAKING CHANGES (read me when you update!)
 - FREQUENTLY ASKED QUESTIONS (FAQ), TIPS
   - How do I update to a newer version of ImGui?
   - Can I have multiple widgets with the same label? Can I have widget without a label? (Yes)
   - I integrated ImGui in my engine and the text or lines are blurry..
   - I integrated ImGui in my engine and some elements are disappearing when I move windows around..
   - How can I load a different font than the default?
   - How can I load multiple fonts?
   - How can I display and input non-latin characters such as Chinese, Japanese, Korean, Cyrillic?
 - ISSUES & TODO-LIST
 - CODE


 MISSION STATEMENT
 =================

 - easy to use to create code-driven and data-driven tools
 - easy to use to create ad hoc short-lived tools and long-lived, more elaborate tools
 - easy to hack and improve
 - minimize screen real-estate usage
 - minimize setup and maintenance
 - minimize state storage on user side
 - portable, minimize dependencies, run on target (consoles, phones, etc.)
 - efficient runtime (NB- we do allocate when "growing" content - creating a window / opening a tree node for the first time, etc. - but a typical frame won't allocate anything)
 - read about immediate-mode gui principles @ http://mollyrocket.com/861, http://mollyrocket.com/forums/index.html

 Designed for developers and content-creators, not the typical end-user! Some of the weaknesses includes:
 - doesn't look fancy, doesn't animate
 - limited layout features, intricate layouts are typically crafted in code
 - occasionally uses statically sized buffers for string manipulations - won't crash, but some very long pieces of text may be clipped. functions like ImGui::TextUnformatted() don't have such restriction.


 END-USER GUIDE
 ==============

 - double-click title bar to collapse window
 - click upper right corner to close a window, available when 'bool* pOpened' is passed to ImGui::Begin()
 - click and drag on lower right corner to resize window
 - click and drag on any empty space to move window
 - double-click/double-tap on lower right corner grip to auto-fit to content
 - TAB/SHIFT+TAB to cycle through keyboard editable fields
 - use mouse wheel to scroll
 - use CTRL+mouse wheel to zoom window contents (if IO.FontAllowScaling is true)
 - CTRL+Click on a slider or drag box to input value as text
 - text editor:
   - Hold SHIFT or use mouse to select text.
   - CTRL+Left/Right to word jump
   - CTRL+Shift+Left/Right to select words
   - CTRL+A our Double-Click to select all
   - CTRL+X,CTRL+C,CTRL+V to use OS clipboard
   - CTRL+Z,CTRL+Y to undo/redo
   - ESCAPE to revert text to its original value
   - You can apply arithmetic operators +,*,/ on numerical values. Use +- to subtract (because - would set a negative value!)


 PROGRAMMER GUIDE
 ================

 - read the FAQ below this section!
 - your code creates the UI, if your code doesn't run the UI is gone! == very dynamic UI, no construction/destructions steps, less data retention on your side, no state duplication, less sync, less bugs.
 - call and read ImGui::ShowTestWindow() for demo code demonstrating most features.
 - see examples/ folder for standalone sample applications. e.g. examples/opengl_example/
 - customization: PushStyleColor()/PushStyleVar() or the style editor to tweak the look of the interface (e.g. if you want a more compact UI or a different color scheme).

 - getting started:
   - initialisation: call ImGui::GetIO() to retrieve the ImGuiIO structure and fill the 'Settings' data.
   - every frame:
      1/ in your mainloop or right after you got your keyboard/mouse info, call ImGui::GetIO() and fill the 'Input' data, then call ImGui::NewFrame().
      2/ use any ImGui function you want between NewFrame() and Render()
      3/ ImGui::Render() to render all the accumulated command-lists. it will call your RenderDrawListFn handler that you set in the IO structure.
   - all rendering information are stored into command-lists until ImGui::Render() is called.
   - ImGui never touches or know about your GPU state. the only function that knows about GPU is the RenderDrawListFn handler that you must provide.
   - effectively it means you can create widgets at any time in your code, regardless of "update" vs "render" considerations.
   - refer to the examples applications in the examples/ folder for instruction on how to setup your code.
   - a typical application skeleton may be:

        //Application init
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = 1920.f;
        io.DisplaySize.y = 1280.f;
        io.IniFilename = "imgui.ini";
        io.RenderDrawListsFn = my_render_function;	//Setup a render function, or set to NULL and call GetDrawData() after Render() to access the render data.
        //TODO: Fill others settings of the io structure

        //Load texture atlas
        unsigned char* pixels;
        int width, height, bytes_per_pixels;
        io.Fonts->GetTexDataAsRGBA32(pixels, &width, &height, &bytes_per_pixels);
        //TODO: copy texture to graphics memory.
        //TODO: store your texture pointer/identifier in 'io.Fonts->TexID'

        //Application main loop
        while (true)
        {
            //1) get low-level input
            //e.g. on Win32, GetKeyboardState(), or poll your events, etc.

            //2) TODO: fill all fields of IO structure and call NewFrame
            ImGuiIO& io = ImGui::GetIO();
            io.DeltaTime = 1.f/60.f;
            io.MousePos = mouse_pos;
            io.MouseDown[0] = mouse_button_0;
            io.KeysDown[i] = ...
            ImGui::NewFrame();

            //3) most of your application code here - you can use any of ImGui::* functions at any point in the frame
            ImGui::Begin("My window");
            ImGui::Text("Hello, world.");
            ImGui::End();
            GameUpdate();
            GameRender();

            //4) render & swap video buffers
            ImGui::Render();
            //swap video buffer, etc.
        }

   - after calling ImGui::NewFrame() you can read back flags from the IO structure to tell how ImGui intends to use your inputs.
     When 'io.WantCaptureMouse' or 'io.WantCaptureKeyboard' flags are set you may want to discard/hide the inputs from the rest of your application.
     When 'io.WantInputsCharacters' is set to may want to notify your OS to popup an on-screen keyboard, if available.


 API BREAKING CHANGES
 ====================

 Occasionally introducing changes that are breaking the API. The breakage are generally minor and easy to fix.
 Here is a change-log of API breaking changes, if you are using one of the functions listed, expect to have to fix some code.
 Also read releases logs https://github.com/ocornut/imgui/releases for more details.

 - 2015/08/29 (1.45) - with the addition of horizontal scrollbar we made various fixes to inconsistencies with dealing with cursor position.
                       GetCursorPos()/SetCursorPos() functions now include the scrolled amount. It shouldn't affect the majority of users, but take note that SetCursorPosX(100.f) puts you at +100 from the starting x position which may include scrolling, not at +100 from the window left side.
                       GetContentRegionMax()/GetWindowContentRegionMin()/GetWindowContentRegionMax() functions allow include the scrolled amount. Typically those were used in cases where no scrolling would happen so it may not be a problem, but watch out!
 - 2015/08/29 (1.45) - renamed style.ScrollbarWidth to style.ScrollbarSize
 - 2015/08/05 (1.44) - split imgui.cpp into extra files: imgui_demo.cpp imgui_draw.cpp imgui_internal.h that you need to add to your project.
 - 2015/07/18 (1.44) - fixed angles in ImDrawList::PathArcTo(), PathArcToFast() (introduced in 1.43) being off by an extra PI for no justifiable reason
 - 2015/07/14 (1.43) - add new ImFontAtlas::AddFont() API. For the old AddFont***, moved the 'font_no' parameter of ImFontAtlas::AddFont** functions to the ImFontConfig structure.
                       you need to render your textured triangles with bilinear filtering to benefit from sub-pixel positioning of text.
 - 2015/07/08 (1.43) - switched rendering data to use indexed rendering. this is saving a fair amount of CPU/GPU and enables us to get anti-aliasing for a marginal cost.
                       this necessary change will break your rendering function! the fix should be very easy. sorry for that :(
                     - if you are using a vanilla copy of one of the imgui_impl_XXXX.cpp provided in the example, you just need to update your copy and you can ignore the rest.
                     - the signature of the io.RenderDrawListsFn handler has changed!
                            ImGui_XXXX_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
                       became:
                            ImGui_XXXX_RenderDrawLists(ImDrawData* draw_data).
                              argument   'cmd_lists'        -> 'draw_data->CmdLists'
                              argument   'cmd_lists_count'  -> 'draw_data->CmdListsCount'
                              ImDrawList 'commands'         -> 'CmdBuffer'
                              ImDrawList 'vtx_buffer'       -> 'VtxBuffer'
                              ImDrawList  n/a               -> 'IdxBuffer' (new)
                              ImDrawCmd  'vtx_count'        -> 'ElemCount'
                              ImDrawCmd  'clipRect'        -> 'ClipRect'
                              ImDrawCmd  'user_callback'    -> 'UserCallback'
                              ImDrawCmd  'texture_id'       -> 'TextureId'
                     - each ImDrawList now contains both a vertex buffer and an index buffer. For each command, render ElemCount/3 triangles using indices from the index buffer.
                     - if you REALLY cannot render indexed primitives, you can call the draw_data->DeIndexAllBuffers() method to de-index the buffers. This is slow and a waste of CPU/GPU. Prefer using indexed rendering!
                     - refer to code in the examples/ folder or ask on the GitHub if you are unsure of how to upgrade. please upgrade!
 - 2015/07/10 (1.43) - changed SameLine() parameters from int to float.
 - 2015/07/02 (1.42) - renamed SetScrollPosHere() to SetScrollFromCursorPos(). Kept inline redirection function (will obsolete).
 - 2015/07/02 (1.42) - renamed GetScrollPosY() to GetScrollY(). Necessary to reduce confusion along with other scrolling functions, because positions (e.g. cursor position) are not equivalent to scrolling amount.
 - 2015/06/14 (1.41) - changed ImageButton() default bgCol parameter from (0,0,0,1) (black) to (0,0,0,0) (transparent) - makes a difference when texture have transparence
 - 2015/06/14 (1.41) - changed Selectable() API from (label, selected, size) to (label, selected, flags, size). Size override should have been rarely be used. Sorry!
 - 2015/05/31 (1.40) - renamed GetWindowCollapsed() to IsWindowCollapsed() for consistency. Kept inline redirection function (will obsolete).
 - 2015/05/31 (1.40) - renamed IsRectClipped() to IsRectVisible() for consistency. Note that return value is opposite! Kept inline redirection function (will obsolete).
 - 2015/05/27 (1.40) - removed the third 'repeat_if_held' parameter from Button() - sorry! it was rarely used and inconsistent. Use PushButtonRepeat(true) / PopButtonRepeat() to enable repeat on desired buttons.
 - 2015/05/11 (1.40) - changed BeginPopup() API, takes a string identifier instead of a bool. ImGui needs to manage the open/closed state of popups. Call OpenPopup() to actually set the "opened" state of a popup. BeginPopup() returns true if the popup is opened.
 - 2015/05/03 (1.40) - removed style.AutoFitPadding, using style.WindowPadding makes more sense (the default values were already the same).
 - 2015/04/13 (1.38) - renamed IsClipped() to IsRectClipped(). Kept inline redirection function (will obsolete).
 - 2015/04/09 (1.38) - renamed ImDrawList::AddArc() to ImDrawList::AddArcFast() for compatibility with future API
 - 2015/04/03 (1.38) - removed ImGuiCol_CheckHovered, ImGuiCol_CheckActive, replaced with the more general ImGuiCol_FrameBgHovered, ImGuiCol_FrameBgActive.
 - 2014/04/03 (1.38) - removed support for passing -FLT_MAX..+FLT_MAX as the range for a SliderFloat(). Use DragFloat() or Inputfloat() instead.
 - 2015/03/17 (1.36) - renamed GetItemBoxMin()/GetItemBoxMax()/IsMouseHoveringBox() to GetItemRectMin()/GetItemRectMax()/IsMouseHoveringRect(). Kept inline redirection function (will obsolete).
 - 2015/03/15 (1.36) - renamed style.TreeNodeSpacing to style.IndentSpacing, ImGuiStyleVar_TreeNodeSpacing to ImGuiStyleVar_IndentSpacing
 - 2015/03/13 (1.36) - renamed GetWindowIsFocused() to IsWindowFocused(). Kept inline redirection function (will obsolete).
 - 2015/03/08 (1.35) - renamed style.ScrollBarWidth to style.ScrollbarWidth (casing)
 - 2015/02/27 (1.34) - renamed OpenNextNode(bool) to SetNextTreeNodeOpened(bool, ImGuiSetCond). Kept inline redirection function (will obsolete).
 - 2015/02/27 (1.34) - renamed ImGuiSetCondition_*** to ImGuiSetCond_***, and _FirstUseThisSession becomes _Once.
 - 2015/02/11 (1.32) - changed text input callback ImGuiTextEditCallback return type from void-->int. reserved for future use, return 0 for now.
 - 2015/02/10 (1.32) - renamed GetItemWidth() to CalcItemWidth() to clarify its evolving behavior
 - 2015/02/08 (1.31) - renamed GetTextLineSpacing() to GetTextLineHeightWithSpacing()
 - 2015/02/01 (1.31) - removed IO.MemReallocFn (unused)
 - 2015/01/19 (1.30) - renamed ImGuiStorage::GetIntPtr()/GetFloatPtr() to GetIntRef()/GetIntRef() because Ptr was conflicting with actual pointer storage functions.
 - 2015/01/11 (1.30) - big font/image API change! now loads TTF file. allow for multiple fonts. no need for a PNG loader.
              (1.30) - removed GetDefaultFontData(). uses io.Fonts->GetTextureData*() API to retrieve uncompressed pixels.
                       this sequence:
                           const void* png_data;
                           unsigned int png_size;
                           ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);
                           //<Copy to GPU>
                       became:
                           unsigned char* pixels;
                           int width, height;
                           io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
                           //<Copy to GPU>
                           io.Fonts->TexID = (your_texture_identifier);
                       you now have much more flexibility to load multiple TTF fonts and manage the texture buffer for internal needs.
                       it is now recommended that you sample the font texture with bilinear interpolation.
              (1.30) - added texture identifier in ImDrawCmd passed to your render function (we can now render images). make sure to set io.Fonts->TexID.
              (1.30) - removed IO.PixelCenterOffset (unnecessary, can be handled in user projection matrix)
              (1.30) - removed ImGui::IsItemFocused() in favor of ImGui::IsItemActive() which handles all widgets
 - 2014/12/10 (1.18) - removed SetNewWindowDefaultPos() in favor of new generic API SetNextWindowPos(pos, ImGuiSetCondition_FirstUseEver)
 - 2014/11/28 (1.17) - moved IO.Font*** options to inside the IO.Font-> structure (FontYOffset, FontTexUvForWhite, FontBaseScale, FontFallbackGlyph)
 - 2014/11/26 (1.17) - reworked syntax of IMGUI_ONCE_UPON_A_FRAME helper macro to increase compiler compatibility
 - 2014/11/07 (1.15) - renamed IsHovered() to IsItemHovered()
 - 2014/10/02 (1.14) - renamed IMGUI_INCLUDE_IMGUI_USER_CPP to IMGUI_INCLUDE_IMGUI_USER_INL and imgui_user.cpp to imgui_user.inl (more IDE friendly)
 - 2014/09/25 (1.13) - removed 'textEnd' parameter from IO.SetClipboardTextFn (the string is now always zero-terminated for simplicity)
 - 2014/09/24 (1.12) - renamed SetFontScale() to SetWindowFontScale()
 - 2014/09/24 (1.12) - moved IM_MALLOC/IM_REALLOC/IM_FREE preprocessor defines to IO.MemAllocFn/IO.MemReallocFn/IO.MemFreeFn
 - 2014/08/30 (1.09) - removed IO.FontHeight (now computed automatically)
 - 2014/08/30 (1.09) - moved IMGUI_FONT_TEX_UV_FOR_WHITE preprocessor define to IO.FontTexUvForWhite
 - 2014/08/28 (1.09) - changed the behavior of IO.PixelCenterOffset following various rendering fixes


 FREQUENTLY ASKED QUESTIONS (FAQ), TIPS
 ======================================

 Q: How do I update to a newer version of ImGui?
 A: Overwrite the following files:
      imgui.cpp
      imgui.h
      imgui_demo.cpp
      imgui_draw.cpp
      imgui_internal.h
      stb_rect_pack.h
      stb_textedit.h
      stb_truetype.h
    Don't overwrite imconfig.h if you have made modification to your copy.
    Check the "API BREAKING CHANGES" sections for a list of occasional API breaking changes. If you have a problem with a function, search for its name
    in the code, there will likely be a comment about it. Please report any issue to the GitHub page!

 Q: Can I have multiple widgets with the same label? Can I have widget without a label? (Yes)
 A: Yes. A primer on the use of labels/IDs in ImGui..

   - Elements that are not clickable, such as Text() items don't need an ID.

   - Interactive widgets require state to be carried over multiple frames (most typically ImGui often needs to remember what is the "active" widget).
     to do so they need an unique ID. unique ID are typically derived from a string label, an integer index or a pointer.

       Button("OK");	//Label = "OK",     ID = hash of "OK"
       Button("Cancel");	//Label = "Cancel", ID = hash of "Cancel"

   - ID are uniquely scoped within windows, tree nodes, etc. so no conflict can happen if you have two buttons called "OK" in two different windows
     or in two different locations of a tree.

   - If you have a same ID twice in the same location, you'll have a conflict:

       Button("OK");
       Button("OK");	//ID collision! Both buttons will be treated as the same.

     Fear not! this is easy to solve and there are many ways to solve it!

   - When passing a label you can optionally specify extra unique ID information within string itself. This helps solving the simpler collision cases.
     use "##" to pass a complement to the ID that won't be visible to the end-user:

       Button("Play");	//Label = "Play",   ID = hash of "Play"
       Button("Play##foo1");	//Label = "Play",   ID = hash of "Play##foo1" (different from above)
       Button("Play##foo2");	//Label = "Play",   ID = hash of "Play##foo2" (different from above)

   - If you want to completely hide the label, but still need an ID:

       Checkbox("##On", &b);	//Label = "",       ID = hash of "##On" (no label!)

   - Occasionally/rarely you might want change a label while preserving a constant ID. This allows you to animate labels.
     For example you may want to include varying information in a window title bar (and windows are uniquely identified by their ID.. obviously)
     Use "###" to pass a label that isn't part of ID:

       Button("Hello###ID";	//Label = "Hello",  ID = hash of "ID"
       Button("World###ID";	//Label = "World",  ID = hash of "ID" (same as above)
       
       sprintf(buf, "My game (%f FPS)###MyGame");
       Begin(buf);	//Variable label,   ID = hash of "MyGame"

   - Use PushID() / PopID() to create scopes and avoid ID conflicts within the same Window.
     This is the most convenient way of distinguishing ID if you are iterating and creating many UI elements.
     You can push a pointer, a string or an integer value. Remember that ID are formed from the concatenation of everything in the ID stack!

       for (int i = 0; i < 100; i++)
       {
         PushID(i);
         Button("Click");	//Label = "Click",  ID = hash of integer + "label" (unique)
         PopID();
       }

       for (int i = 0; i < 100; i++)
       {
         MyObject* obj = Objects[i];
         PushID(obj);
         Button("Click");	//Label = "Click",  ID = hash of pointer + "label" (unique)
         PopID();
       }

       for (int i = 0; i < 100; i++)
       {
         MyObject* obj = Objects[i];
         PushID(obj->Name);
         Button("Click");	//Label = "Click",  ID = hash of string + "label" (unique)
         PopID();
       }

   - More example showing that you can stack multiple prefixes into the ID stack:

       Button("Click");	//Label = "Click",  ID = hash of "Click"
       PushID("node");
       Button("Click");	//Label = "Click",  ID = hash of "node" + "Click"
         PushID(my_ptr);
           Button("Click");	//Label = "Click",  ID = hash of "node" + ptr + "Click"
         PopID();
       PopID();

   - Tree nodes implicitly creates a scope for you by calling PushID().

       Button("Click");	//Label = "Click",  ID = hash of "Click"
       if (TreeNode("node"))
       {
         Button("Click");	//Label = "Click",  ID = hash of "node" + "Click"
         TreePop();
       }

   - When working with trees, ID are used to preserve the opened/closed state of each tree node.
     Depending on your use cases you may want to use strings, indices or pointers as ID.
      e.g. when displaying a single object that may change over time (1-1 relationship), using a static string as ID will preserve your node open/closed state when the targeted object change.
      e.g. when displaying a list of objects, using indices or pointers as ID will preserve the node open/closed state differently. experiment and see what makes more sense!

 Q: I integrated ImGui in my engine and the text or lines are blurry..
 A: In your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f).
    Also make sure your orthographic projection matrix and io.DisplaySize matches your actual framebuffer dimension.

 Q. I integrated ImGui in my engine and some elements are disappearing when I move windows around..
    Most likely you are mishandling the clipping rectangles in your render function. Rectangles provided by ImGui are defined as (x1,y1,x2,y2) and NOT as (x1,y1,width,height).

 Q: How can I load a different font than the default? (default is an embedded version of ProggyClean.ttf, rendered at size 13)
 A: Use the font atlas to load the TTF file you want:

      ImGuiIO& io = ImGui::GetIO();
      io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels);
      io.Fonts->GetTexDataAsRGBA32() or GetTexDataAsAlpha8()

 Q: How can I load multiple fonts?
 A: Use the font atlas to pack them into a single texture:

      ImGuiIO& io = ImGui::GetIO();
      ImFont* font0 = io.Fonts->AddFontDefault();
      ImFont* font1 = io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels);
      ImFont* font2 = io.Fonts->AddFontFromFileTTF("myfontfile2.ttf", size_in_pixels);
      io.Fonts->GetTexDataAsRGBA32() or GetTexDataAsAlpha8()
      //the first loaded font gets used by default
      //use ImGui::PushFont()/ImGui::PopFont() to change the font at runtime

      //Options
      ImFontConfig config;
      config.OversampleH = 3;
      config.OversampleV = 3;
      config.GlyphExtraSpacing.x = 1.f;
      io.Fonts->LoadFromFileTTF("myfontfile.ttf", size_pixels, &config);

      //Combine multiple fonts into one
      ImWchar ranges[] = { 0xf000, 0xf3ff, 0 };
      ImFontConfig config;
      config.MergeMode = true;
      io.Fonts->AddFontDefault();
      io.Fonts->LoadFromFileTTF("fontawesome-webfont.ttf", 16.f, &config, ranges);
      io.Fonts->LoadFromFileTTF("myfontfile.ttf", size_pixels, NULL, &config, io.Fonts->GetGlyphRangesJapanese());

    Read extra_fonts/README.txt or ImFontAtlas class for more details.

 Q: How can I display and input non-Latin characters such as Chinese, Japanese, Korean, Cyrillic?
 A: When loading a font, pass custom Unicode ranges to specify the glyphs to load. ImGui will support UTF-8 encoding across the board.
    Character input depends on you passing the right character code to io.AddInputCharacter(). The example applications do that.

      io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels, NULL, io.Fonts->GetGlyphRangesJapanese());  //Load Japanese characters
      io.Fonts->GetTexDataAsRGBA32() or GetTexDataAsAlpha8()
      io.ImeWindowHandle = MY_HWND;	//To input using Microsoft IME, give ImGui the hwnd of your application

 - tip: the construct 'IMGUI_ONCE_UPON_A_FRAME { ... }' will run the block of code only once a frame. You can use it to quickly add custom UI in the middle of a deep nested inner loop in your code.
 - tip: you can create widgets without a Begin()/End() block, they will go in an implicit window called "Debug"
 - tip: you can call Begin() multiple times with the same name during the same frame, it will keep appending to the same window. this is also useful to set yourself in the context of another window (to get/set other settings)
 - tip: you can call Render() multiple times (e.g for VR renders).
 - tip: call and read the ShowTestWindow() code in imgui_demo.cpp for more example of how to use ImGui!


 ISSUES & TODO-LIST
 ==================
 Issue numbers (#) refer to github issues.
 The list below consist mostly of notes of things to do before they are requested/discussed by users (at that point it usually happens on the github)

 - window: maximum window size settings (per-axis). for large popups in particular user may not want the popup to fill all space.
 - window: add a way for very transient windows (non-saved, temporary overlay over hundreds of objects) to "clean" up from the global window list. perhaps a lightweight explicit cleanup pass.
 - window: auto-fit feedback loop when user relies on any dynamic layout (window width multiplier, column). clarify.
 - window: allow resizing of child windows (possibly given min/max for each axis?)
 - window: background options for child windows, border option (disable rounding)
 - window: add a way to clear an existing window instead of appending (e.g. for tooltip override using a consistent api rather than the deferred tooltip)
 - window: resizing from any sides? + mouse cursor directives for app.
!- window: begin with *pOpened == false should return false.
 - window: get size/pos helpers given names (see discussion in #249)
 - window: a collapsed window can be stuck behind the main menu bar?
 - window: detect extra End() call that pop the "Debug" window out and assert at call site instead of later.
!- scrolling: allow immediately effective change of scroll if we haven't appended items yet
 - splitter: formalize the splitter idiom into an official api (we want to handle n-way split)
 - widgets: display mode: widget-label, label-widget (aligned on column or using fixed size), label-newline-tab-widget etc.
 - widgets: clean up widgets internal toward exposing everything.
 - widgets: add disabled and read-only modes (#211)
 - main: considering adding EndFrame()/Init(). some constructs are awkward in the implementation because of the lack of them.
 - main: IsItemHovered() make it more consistent for various type of widgets, widgets with multiple components, etc. also effectively IsHovered() region sometimes differs from hot region, e.g tree nodes
 - main: IsItemHovered() info stored in a stack? so that 'if TreeNode() { Text; TreePop; } if IsHovered' return the hover state of the TreeNode?
 - input text: add ImGuiInputTextFlags_EnterToApply? (off #218)
 - input text multi-line: don't directly call AddText() which does an unnecessary vertex reserve for character count prior to clipping. and/or more line-based clipping to AddText(). and/or reorganize TextUnformatted/RenderText for more efficiency for large text (e.g TextUnformatted could clip and log separately, etc).
 - input text multi-line: way to dynamically grow the buffer without forcing the user to initially allocate for worse case (follow up on #200)
 - input text multi-line: line numbers? status bar? (follow up on #200)
 - input number: optional range min/max for Input*() functions
 - input number: holding [-]/[+] buttons could increase the step speed non-linearly (or user-controlled)
 - input number: use mouse wheel to step up/down
 - input number: applying arithmetics ops (+,-,*,/) messes up with text edit undo stack.
 - text: proper alignment options
 - image/image button: misalignment on padded/bordered button?
 - image/image button: parameters are confusing, image() has tintCol,borderCol whereas imagebutton() has bgCol/tintCol. Even thou they are different parameters ordering could be more consistent. can we fix that?
 - layout: horizontal layout helper (#97)
 - layout: horizontal flow until no space left (#404)
 - layout: more generic alignment state (left/right/centered) for single items?
 - layout: clean up the InputFloatN/SliderFloatN/ColorEdit4 layout code. item width should include frame padding.
 - columns: separator function or parameter that works within the column (currently Separator() bypass all columns) (#125)
 - columns: declare column set (each column: fixed size, %, fill, distribute default size among fills) (#125)
 - columns: columns header to act as button (~sort op) and allow resize/reorder (#125)
 - columns: user specify columns size (#125)
 - combo: sparse combo boxes (via function call?)
 - combo: contents should extends to fit label if combo widget is small
 - combo/listbox: keyboard control. need InputText-like non-active focus + key handling. considering keyboard for custom listbox (pr #203)
 - listbox: multiple selection
 - listbox: user may want to initial scroll to focus on the one selected value?
 - listbox: keyboard navigation.
 - listbox: scrolling should track modified selection.
!- popups/menus: clarify usage of popups id, how MenuItem/Selectable closing parent popups affects the ID, etc. this is quite fishy needs improvement! (#331, #402)
 - popups: add variant using global identifier similar to Begin/End (#402)
 - popups: border options. richer api like BeginChild() perhaps? (#197)
 - menus: local shortcuts, global shortcuts (#126)
 - menus: icons
 - menus: menubars: some sort of priority / effect of main menu-bar on desktop size?
 - statusbar: add a per-window status bar helper similar to what menubar does.
 - tabs
 - separator: separator on the initial position of a window is not visible (cursorpos.y <= clippos.y)
 - gauge: various forms of gauge/loading bars widgets
 - color: add a better color picker
 - plot: PlotLines() should use the polygon-stroke facilities (currently issues with averaging normals)
 - plot: make it easier for user to draw extra stuff into the graph (e.g: draw basis, highlight certain points, 2d plots, multiple plots)
 - plot: "smooth" automatic scale over time, user give an input 0.0(full user scale) 1.0(full derived from value)
 - plot: add a helper e.g. Plot(char* label, float value, float time_span=2.f) that stores values and Plot them for you - probably another function name. and/or automatically allow to plot ANY displayed value (more reliance on stable ID)
 - applet: file selection widget (as an example)
 - slider: allow using the [-]/[+] buttons used by InputFloat()/InputInt()
 - slider: initial absolute click is imprecise. change to relative movement slider (same as scrollbar).
 - slider: add dragging-based widgets to edit values with mouse (on 2 axises), saving screen real-estate.
 - slider: tint background based on value (e.g. vMin -> vMax, or use 0.f either side of the sign)
 - slider & drag: int data passing through a float
 - drag float: up/down axis
 - text edit: clean up the mess caused by converting UTF-8 <> wchar. the code is rather inefficient right now.
 - text edit: centered text for slider as input text so it matches typical positioning.
 - text edit: flag to disable live update of the user buffer.
 - text edit: field resize behavior - field could stretch when being edited? hover tooltip shows more text?
 - tree node / optimization: avoid formatting when clipped.
 - tree node: tree-node/header right-most side doesn't take account of horizontal scrolling.
 - tree node: add treenode/treepush int variants? because (void*) cast from int warns on some platforms/settings
 - textwrapped: figure out better way to use TextWrapped() in an always auto-resize context (tooltip, etc.) (git issue #249)
 - settings: write more decent code to allow saving/loading new fields
 - settings: api for per-tool simple persistent data (bool,int,float,columns sizes,etc.) in .ini file
 - style: store rounded corners in texture to use 1 quad per corner (filled and wireframe). so rounding have minor cost.
 - style: color-box not always square?
 - style: a concept of "compact style" that the end-user can easily rely on (e.g. PushStyleCompact()?) that maps to other settings? avoid implementing duplicate helpers such as SmallCheckbox(), etc.
 - text: simple markup language for color change?
 - font: helper to add glyph redirect/replacements (e.g. redirect alternate apostrophe unicode code points to ascii one, etc.)
 - log: LogButtons() options for specifying depth and/or hiding depth slider
 - log: have more control over the log scope (e.g. stop logging when leaving current tree node scope)
 - log: be able to log anything (e.g. right-click on a window/tree-node, shows context menu? log into tty/file/clipboard)
 - log: let user copy any window content to clipboard easily (CTRL+C on windows? while moving it? context menu?). code is commented because it fails with multiple Begin/End pairs.
 - filters: set a current filter that tree node can automatically query to hide themselves
 - filters: handle wildcards (with implicit leading/trailing *), regexps
 - shortcuts: add a shortcut api, e.g. parse "&Save" and/or "Save (CTRL+S)", pass in to widgets or provide simple ways to use (button=activate, input=focus)
!- keyboard: tooltip & combo boxes are messing up / not honoring keyboard tabbing
 - keyboard: full keyboard navigation and focus.
 - focus: SetKeyboardFocusHere() on with >= 0 offset could be done on same frame (else latch and modulate on beginning of next frame)
 - input: rework IO to be able to pass actual events to fix temporal aliasing issues.
 - input: support track pad style scrolling & slider edit.
 - misc: provide a way to compile out the entire implementation while providing a dummy API (e.g. #define IMGUI_DUMMY_IMPL)
 - misc: double-clicking on title bar to minimize isn't consistent, perhaps move to single-click on left-most collapse icon?
 - style editor: have a more global HSV setter (e.g. alter hue on all elements). consider replacing active/hovered by offset in HSV space?
 - style editor: color child window height expressed in multiple of line height.
!- examples: directx9/directx11: resizing window duplicate the font data :(
 - optimization: use another hash function than crc32, e.g. FNV1a
 - optimization/render: merge command-lists with same clip-rect into one even if they aren't sequential? (as long as in-between clip rectangle don't overlap)?
 - optimization: turn some the various stack vectors into statically-sized arrays
 - optimization: better clipping for multi-component widgets
*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include <ctype.h>      //toupper, isprint
#include <math.h>       //sqrtf, fabsf, fmodf, powf, cosf, sinf, floorf, ceilf
#include <stdlib.h>     //NULL, malloc, free, qsort, atoi
#include <stdio.h>      //vsnprintf, sscanf, printf
#include <new>          //new (ptr)
#if defined(_MSC_VER) && _MSC_VER <= 1500 //MSVC 2008 or earlier
#include <stddef.h>     //intptr_t
#else
#include <stdint.h>     //intptr_t
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4127) //condition expression is constant
#pragma warning (disable: 4505) //unreferenced local function has been removed (stb stuff)
#pragma warning (disable: 4996) //'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#define snprintf _snprintf
#endif

//Clang warnings with -Weverything
#ifdef __clang__
#pragma clang diagnostic ignored "-Wold-style-cast"         //warning : use of old-style cast                              //yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"            //warning : comparing floating point with == or != is unsafe   //storing and comparing against same constants ok.
#pragma clang diagnostic ignored "-Wformat-nonliteral"      //warning : format string is not a string literal              //passing non-literal to vsnformat(). yes, user passing incorrect format strings can crash the code.
#pragma clang diagnostic ignored "-Wexit-time-destructors"  //warning : declaration requires an exit-time destructor       //exit-time destruction order is undefined. if MemFree() leads to users code that has been disabled before exit it might cause problems. ImGui coding style welcomes static/globals.
#pragma clang diagnostic ignored "-Wglobal-constructors"    //warning : declaration requires a global destructor           //similar to above, not sure what the exact difference it.
#pragma clang diagnostic ignored "-Wsign-conversion"        //warning : implicit conversion changes signedness             //
#pragma clang diagnostic ignored "-Wmissing-noreturn"       //warning : function xx could be declared with attribute 'noreturn' warning    //GetDefaultFontData() asserts which some implementation makes it never return.
#pragma clang diagnostic ignored "-Wdeprecated-declarations"//warning : 'xx' is deprecated: The POSIX name for this item.. //for strdup used in demo code (so user can copy & paste the code)
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast" //warning : cast to 'void *' from smaller integer type 'int'
#endif
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"          //warning: 'xxxx' defined but not used
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"      //warning: cast to pointer from integer of different size
#endif

//-------------------------------------------------------------------------
//Forward Declarations
//-------------------------------------------------------------------------

static void LogRenderedText(const ImVec2& refPos, const char* text, const char* textEnd = NULL);
static const char* FindTextDisplayEnd(const char* text, const char* textEnd = NULL);

static void PushMultiItemsWidths(int components, float wFull = 0.f);
static float GetDraggedColumnOffset(int columnIndex);

static bool IsKeyPressedMap(ImGuiKey key, bool repeat = true);

static void SetCurrentFont(ImFont* font);
static void SetCurrentWindow(ImGuiWindow* window);
static void SetWindowScrollY(ImGuiWindow* window, float newScrollY);
static void SetWindowPos(ImGuiWindow* window, const ImVec2& pos, ImGuiSetCond cond);
static void SetWindowSize(ImGuiWindow* window, const ImVec2& size, ImGuiSetCond cond);
static void SetWindowCollapsed(ImGuiWindow* window, bool collapsed, ImGuiSetCond cond);
static ImGuiWindow* FindWindowByName(const char* name);
static ImGuiWindow* FindHoveredWindow(ImVec2 pos, bool excludingChilds);
static ImGuiWindow* CreateNewWindow(const char* name, ImVec2 size, ImGuiWindowFlags flags);
static inline bool IsWindowContentHoverable(ImGuiWindow* window);
static void ClearSetNextWindowData();
static void CheckStacksSize(ImGuiWindow* window, bool write);
static void Scrollbar(ImGuiWindow* window, bool horizontal);
static bool CloseWindowButton(bool* pOpened);

static void AddDrawListToRenderList(ImVector<ImDrawList*>& outRenderList, ImDrawList* drawList);
static void AddWindowToRenderList(ImVector<ImDrawList*>& outRenderList, ImGuiWindow* window);
static void AddWindowToSortedBuffer(ImVector<ImGuiWindow*>& outSortedWindows, ImGuiWindow* window);

static ImGuiIniData*FindWindowSettings(const char* name);
static ImGuiIniData*AddWindowSettings(const char* name);
static void LoadSettings();
static void SaveSettings();
static void MarkSettingsDirty();

static void PushClipRect(const ImRect& clipRect, bool clippedByCurrent = true);
static void PushColumnClipRect(int columnIndex = -1);
static void PopClipRect();
static ImRect GetVisibleRect();

static bool BeginPopupEx(const char* strID, ImGuiWindowFlags extraFlags);
static void CloseInactivePopups();
static void ClosePopupToLevel(int remaining);
static void ClosePopup(ImGuiID id);
static bool IsPopupOpen(ImGuiID id);
static ImGuiWindow* GetFrontMostModalRootWindow();
static ImVec2 FindBestPopupWindowPos(const ImVec2& basePos, const ImVec2& size, ImGuiWindowFlags flags, int* lastDir, const ImRect& rInner);

static bool InputTextFilterCharacter(unsigned int* pChar, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* userData);
static int InputTextCalcTextLenAndLineCount(const char* textBegin, const char** outTextEnd);
static ImVec2 InputTextCalcTextSizeW(const ImWchar* textBegin, const ImWchar* textEnd, const ImWchar** remaining = NULL, ImVec2* outOffset = NULL, bool stopOnNewLine = false);

static inline void DataTypeFormatString(ImGuiDataType dataType, void* dataPtr, const char* displayFormat, char* buf, int bufSize);
static inline void DataTypeFormatString(ImGuiDataType dataType, void* dataPtr, int decimalPrecision, char* buf, int bufSize);
static void DataTypeApplyOp(ImGuiDataType dataType, int op, void* value1, const void* value2);
static void DataTypeApplyOpFromText(const char* buf, const char* initialValueBuf, ImGuiDataType dataType, void* dataPtr, const char* scalarFormat);

//-----------------------------------------------------------------------------
//Platform dependent default implementations
//-----------------------------------------------------------------------------

static const char* GetClipboardTextFnDefaultImpl();
static void SetClipboardTextFnDefaultImpl(const char* text);
static void ImeSetInputScreenPosFnDefaultImpl(int x, int y);

//-----------------------------------------------------------------------------
//Context
//-----------------------------------------------------------------------------

//We access everything through this pointer (always assumed to be != NULL)
//You can swap the pointer to a different context by calling ImGui::SetInternalState()
static ImGuiState GImDefaultState;
ImGuiState* GImGui = &GImDefaultState;

//Statically allocated default font atlas. This is merely a maneuver to keep ImFontAtlas definition at the bottom of the .h file (otherwise it'd be inside ImGuiIO)
//Also we wouldn't be able to new() one at this point, before users may define IO.MemAllocFn.
static ImFontAtlas GImDefaultFontAtlas;

//-----------------------------------------------------------------------------
//User facing structures
//-----------------------------------------------------------------------------

ImGuiStyle::ImGuiStyle()
{
	Alpha = 1.f;	//Global alpha applies to everything in ImGui
	WindowPadding = ImVec2(8, 8);	//Padding within a window
	WindowMinSize = ImVec2(32, 32);	//Minimum window size
	WindowRounding = 9.f;	//Radius of window corners rounding. Set to 0.f to have rectangular windows
	WindowTitleAlign = ImGuiAlign_Left;	//Alignment for title bar text
	ChildWindowRounding = 0.f;	//Radius of child window corners rounding. Set to 0.f to have rectangular windows
	FramePadding = ImVec2(4, 3);	//Padding within a framed rectangle (used by most widgets)
	FrameRounding = 0.f;	//Radius of frame corners rounding. Set to 0.f to have rectangular frames (used by most widgets).
	ItemSpacing = ImVec2(8, 4);	//Horizontal and vertical spacing between widgets/lines
	ItemInnerSpacing = ImVec2(4, 4);	//Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
	TouchExtraPadding = ImVec2(0, 0);	//Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
	WindowFillAlphaDefault = 0.7f;	//Default alpha of window background, if not specified in ImGui::Begin()
	IndentSpacing = 22.f;	//Horizontal spacing when e.g. entering a tree node
	ColumnsMinSpacing = 6.f;	//Minimum horizontal spacing between two columns
	ScrollbarSize = 16.f;	//Width of the vertical scrollbar, Height of the horizontal scrollbar
	ScrollbarRounding = 9.f;	//Radius of grab corners rounding for scrollbar
	GrabMinSize = 10.f;	//Minimum width/height of a grab box for slider/scrollbar
	GrabRounding = 0.f;	//Radius of grabs corners rounding. Set to 0.f to have rectangular slider grabs.
	DisplayWindowPadding = ImVec2(22, 22);	//Window positions are clamped to be visible within the display area by at least this amount. Only covers regular windows.
	DisplaySafeAreaPadding = ImVec2(4, 4);	//If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
	AntiAliasedLines = true;	//Enable anti-aliasing on lines/borders. Disable if you are really short on CPU/GPU.
	AntiAliasedShapes = true;	//Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
	CurveTessellationTol = 1.25f;	//Tessellation tolerance. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.

	Colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 1.f);
	Colors[ImGuiCol_TextDisabled] = ImVec4(0.6f, 0.6f, 0.6f, 1.f);
	Colors[ImGuiCol_WindowBg] = ImVec4(0.f, 0.f, 0.f, 1.f);
	Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
	Colors[ImGuiCol_Border] = ImVec4(0.7f, 0.7f, 0.7f, 0.65f);
	Colors[ImGuiCol_BorderShadow] = ImVec4(0.f, 0.f, 0.f, 0.f);
	Colors[ImGuiCol_FrameBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.3f);	//Background of checkbox, radio button, plot, slider, text input
	Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.9f, 0.8f, 0.8f, 0.4f);
	Colors[ImGuiCol_FrameBgActive] = ImVec4(0.9f, 0.65f, 0.65f, 0.45f);
	Colors[ImGuiCol_TitleBg] = ImVec4(0.5f, 0.5f, 1.f, 0.45f);
	Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.4f, 0.4f, 0.8f, 0.2f);
	Colors[ImGuiCol_TitleBgActive] = ImVec4(0.5f, 0.5f, 1.f, 0.55f);
	Colors[ImGuiCol_MenuBarBg] = ImVec4(0.4f, 0.4f, 0.55f, 0.8f);
	Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2f, 0.25f, 0.3f, 0.6f);
	Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.4f, 0.4f, 0.8f, 0.3f);
	Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4f, 0.4f, 0.8f, 0.4f);
	Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.8f, 0.5f, 0.5f, 0.4f);
	Colors[ImGuiCol_ComboBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.99f);
	Colors[ImGuiCol_CheckMark] = ImVec4(0.9f, 0.9f, 0.9f, 0.5f);
	Colors[ImGuiCol_SliderGrab] = ImVec4(1.f, 1.f, 1.f, 0.3f);
	Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8f, 0.5f, 0.5f, 1.f);
	Colors[ImGuiCol_Button] = ImVec4(0.67f, 0.4f, 0.4f, 0.6f);
	Colors[ImGuiCol_ButtonHovered] = ImVec4(0.67f, 0.4f, 0.4f, 1.f);
	Colors[ImGuiCol_ButtonActive] = ImVec4(0.8f, 0.5f, 0.5f, 1.f);
	Colors[ImGuiCol_Header] = ImVec4(0.4f, 0.4f, 0.9f, 0.45f);
	Colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.45f, 0.9f, 0.8f);
	Colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.87f, 0.8f);
	Colors[ImGuiCol_Column] = ImVec4(0.5f, 0.5f, 0.5f, 1.f);
	Colors[ImGuiCol_ColumnHovered] = ImVec4(0.7f, 0.6f, 0.6f, 1.f);
	Colors[ImGuiCol_ColumnActive] = ImVec4(0.9f, 0.7f, 0.7f, 1.f);
	Colors[ImGuiCol_ResizeGrip] = ImVec4(1.f, 1.f, 1.f, 0.3f);
	Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.f, 1.f, 1.f, 0.6f);
	Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.f, 1.f, 1.f, 0.9f);
	Colors[ImGuiCol_CloseButton] = ImVec4(0.5f, 0.5f, 0.9f, 0.5f);
	Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.7f, 0.7f, 0.9f, 0.6f);
	Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.7f, 0.7f, 0.7f, 1.f);
	Colors[ImGuiCol_PlotLines] = ImVec4(1.f, 1.f, 1.f, 1.f);
	Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.9f, 0.7f, 0.f, 1.f);
	Colors[ImGuiCol_PlotHistogram] = ImVec4(0.9f, 0.7f, 0.f, 1.f);
	Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.f, 0.6f, 0.f, 1.f);
	Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.f, 0.f, 1.f, 0.35f);
	Colors[ImGuiCol_TooltipBg] = ImVec4(0.05f, 0.05f, 0.1f, 0.9f);
	Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.2f, 0.2f, 0.2f, 0.35f);
}

ImGuiIO::ImGuiIO()
{
	//Most fields are initialized with zero
	memset(this, 0, sizeof(*this));

	DisplaySize = ImVec2(-1.f, -1.f);
	DeltaTime = 1.f / 60.f;
	IniSavingRate = 5.f;
	IniFilename = "imgui.ini";
	LogFilename = "imgui_log.txt";
	Fonts = &GImDefaultFontAtlas;
	FontGlobalScale = 1.f;
	DisplayFramebufferScale = ImVec2(1.f, 1.f);
	MousePos = ImVec2(-1, -1);
	MousePosPrev = ImVec2(-1, -1);
	MouseDoubleClickTime = 0.3f;
	MouseDoubleClickMaxDist = 6.f;
	MouseDragThreshold = 6.f;
	for (int i = 0; i < IM_ARRAYSIZE(MouseDownDuration); i++)
		MouseDownDuration[i] = MouseDownDurationPrev[i] = -1.f;
	for (int i = 0; i < IM_ARRAYSIZE(KeysDownDuration); i++)
		KeysDownDuration[i] = KeysDownDurationPrev[i] = -1.f;
	for (int i = 0; i < ImGuiKey_COUNT; i++)
		KeyMap[i] = -1;
	KeyRepeatDelay = 0.25f;
	KeyRepeatRate = 0.05f;
	UserData = NULL;

	//User functions
	RenderDrawListsFn = NULL;
	MemAllocFn = malloc;
	MemFreeFn = free;
	GetClipboardTextFn = GetClipboardTextFnDefaultImpl;	//Platform dependent default implementations
	SetClipboardTextFn = SetClipboardTextFnDefaultImpl;
	ImeSetInputScreenPosFn = ImeSetInputScreenPosFnDefaultImpl;
}

//Pass in translated ASCII characters for text input.
//- with glfw you can get those from the callback set in glfwSetCharCallback()
//- on Windows you can get those using ToAscii+keyboard state, or via the WM_CHAR message
void ImGuiIO::AddInputCharacter(ImWchar c)
{
	const int n = ImStrlenW(InputCharacters);
	if (n + 1 < IM_ARRAYSIZE(InputCharacters))
	{
		InputCharacters[n] = c;
		InputCharacters[n + 1] = '\0';
	}
}

void ImGuiIO::AddInputCharactersUTF8(const char* utf8Chars)
{
	//We can't pass more wchars than ImGuiIO::InputCharacters[] can hold so don't convert more
	const int wcharsBufLen = sizeof(ImGuiIO::InputCharacters) / sizeof(ImWchar);
	ImWchar wchars[wcharsBufLen];
	ImTextStrFromUtf8(wchars, wcharsBufLen, utf8Chars, NULL);
	for (int i = 0; wchars[i] != 0 && i < wcharsBufLen; i++)
		AddInputCharacter(wchars[i]);
}

//-----------------------------------------------------------------------------
//HELPERS
//-----------------------------------------------------------------------------

#define IM_F32_TO_INT8(_VAL)  ((int)((_VAL) * 255.f + 0.5f))

#define IM_INT_MIN  (-2147483647-1)
#define IM_INT_MAX  (2147483647)

//Play it nice with Windows users. Notepad in 2015 still doesn't display text data with Unix-style \n.
#ifdef _WIN32
#define IM_NEWLINE "\r\n"
#else
#define IM_NEWLINE "\n"
#endif

bool ImIsPointInTriangle(const ImVec2& p, const ImVec2& a, const ImVec2& b, const ImVec2& c)
{
	bool b1 = ((p.x - b.x) * (a.y - b.y) - (p.y - b.y) * (a.x - b.x)) < 0.f;
	bool b2 = ((p.x - c.x) * (b.y - c.y) - (p.y - c.y) * (b.x - c.x)) < 0.f;
	bool b3 = ((p.x - a.x) * (c.y - a.y) - (p.y - a.y) * (c.x - a.x)) < 0.f;
	return ((b1 == b2) && (b2 == b3));
}

int ImStricmp(const char* str1, const char* str2)
{
	int d;
	while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1)
	{
		str1++;
		str2++;
	}
	return d;
}

int ImStrnicmp(const char* str1, const char* str2, int count)
{
	int d = 0;
	while (count > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1)
	{
		str1++;
		str2++;
		count--;
	}
	return d;
}

char* ImStrdup(const char *str)
{
	char *buff = (char*)ImGui::MemAlloc(strlen(str) + 1);
	IM_ASSERT(buff);
	strcpy(buff, str);
	return buff;
}

int ImStrlenW(const ImWchar* str)
{
	int n = 0;
	while (*str++)
		n++;
	return n;
}

const ImWchar* ImStrbolW(const ImWchar* bufMidLine, const ImWchar* bufBegin)	//find beginning-of-line
{
	while (bufMidLine > bufBegin && bufMidLine[-1] != '\n')
		bufMidLine--;
	return bufMidLine;
}

const char* ImStristr(const char* haystack, const char* haystackEnd, const char* needle, const char* needleEnd)
{
	if (!needleEnd)
		needleEnd = needle + strlen(needle);

	const char un0 = (char)toupper(*needle);
	while ((!haystackEnd && *haystack) || (haystackEnd && haystack < haystackEnd))
	{
		if (toupper(*haystack) == un0)
		{
			const char* b = needle + 1;
			for (const char* a = haystack + 1; b < needleEnd; a++, b++)
			{
				if (toupper(*a) != toupper(*b))
					break;
			}
			if (b == needleEnd)
				return haystack;
		}
		haystack++;
	}
	return NULL;
}

int ImFormatString(char* buf, int bufSize, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int w = vsnprintf(buf, bufSize, fmt, args);
	va_end(args);
	buf[bufSize - 1] = 0;
	return (w == -1) ? bufSize : w;
}

int ImFormatStringV(char* buf, int bufSize, const char* fmt, va_list args)
{
	int w = vsnprintf(buf, bufSize, fmt, args);
	buf[bufSize - 1] = 0;
	return (w == -1) ? bufSize : w;
}

//Pass dataSize==0 for zero-terminated strings
//FIXME-OPT: Replace with e.g. FNV1a hash? CRC32 pretty much randomly access 1KB. Need to do proper measurements.
ImU32 ImHash(const void* data, int dataSize, ImU32 seed)
{
	static ImU32 crc32Lut[256] = { 0 };
	if (!crc32Lut[1])
	{
		const ImU32 polynomial = 0xEDB88320;
		for (ImU32 i = 0; i < 256; i++)
		{
			ImU32 crc = i;
			for (ImU32 j = 0; j < 8; j++)
				crc = (crc >> 1) ^ (ImU32(-int(crc & 1)) & polynomial);
			crc32Lut[i] = crc;
		}
	}

	seed = ~seed;
	ImU32 crc = seed;
	const unsigned char* current = (const unsigned char*)data;

	if (dataSize > 0)
	{
		//Known size
		while (dataSize--)
			crc = (crc >> 8) ^ crc32Lut[(crc & 0xFF) ^ *current++];
	}
	else
	{
		//Zero-terminated string
		while (unsigned char c = *current++)
		{
			//We support a syntax of "label###id" where only "###id" is included in the hash, and only "label" gets displayed.
			//Because this syntax is rarely used we are optimizing for the common case.
			//- If we reach ### in the string we discard the hash so far and reset to the seed.
			//- We don't do 'current += 2;	continue;' after handling ### to keep the code smaller.
			if (c == '#' && current[0] == '#' && current[1] == '#')
				crc = seed;

			crc = (crc >> 8) ^ crc32Lut[(crc & 0xFF) ^ c];
		}
	}
	return ~crc;
}

//-----------------------------------------------------------------------------
//ImText* helpers
//-----------------------------------------------------------------------------

//Convert UTF-8 to 32-bits character, process single character input.
//Based on stb_from_utf8() from github.com/nothings/stb/
//We handle UTF-8 decoding error by skipping forward.
int ImTextCharFromUtf8(unsigned int* outChar, const char* inText, const char* inTextEnd)
{
	unsigned int c = (unsigned int)-1;
	const unsigned char* str = (const unsigned char*)inText;
	if (!(*str & 0x80))
	{
		c = (unsigned int)(*str++);
		*outChar = c;
		return 1;
	}
	if ((*str & 0xe0) == 0xc0)
	{
		*outChar = 0xFFFD;	//will be invalid but not end of string
		if (inTextEnd && inTextEnd - (const char*)str < 2)
			return 1;
		if (*str < 0xc2)
			return 2;
		c = (unsigned int)((*str++ & 0x1f) << 6);
		if ((*str & 0xc0) != 0x80)
			return 2;
		c += (*str++ & 0x3f);
		*outChar = c;
		return 2;
	}
	if ((*str & 0xf0) == 0xe0)
	{
		*outChar = 0xFFFD;	//will be invalid but not end of string
		if (inTextEnd && inTextEnd - (const char*)str < 3)
			return 1;
		if (*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf))
			return 3;
		if (*str == 0xed && str[1] > 0x9f)
			return 3;	//str[1] < 0x80 is checked below
		c = (unsigned int)((*str++ & 0x0f) << 12);
		if ((*str & 0xc0) != 0x80)
			return 3;
		c += (unsigned int)((*str++ & 0x3f) << 6);
		if ((*str & 0xc0) != 0x80)
			return 3;
		c += (*str++ & 0x3f);
		*outChar = c;
		return 3;
	}
	if ((*str & 0xf8) == 0xf0)
	{
		*outChar = 0xFFFD;	//will be invalid but not end of string
		if (inTextEnd && inTextEnd - (const char*)str < 4)
			return 1;
		if (*str > 0xf4)
			return 4;
		if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf))
			return 4;
		if (*str == 0xf4 && str[1] > 0x8f)
			return 4;	//str[1] < 0x80 is checked below
		c = (unsigned int)((*str++ & 0x07) << 18);
		if ((*str & 0xc0) != 0x80)
			return 4;
		c += (unsigned int)((*str++ & 0x3f) << 12);
		if ((*str & 0xc0) != 0x80)
			return 4;
		c += (unsigned int)((*str++ & 0x3f) << 6);
		if ((*str & 0xc0) != 0x80)
			return 4;
		c += (*str++ & 0x3f);
		//utf-8 encodings of values used in surrogate pairs are invalid
		if ((c & 0xFFFFF800) == 0xD800)
			return 4;
		*outChar = c;
		return 4;
	}
	*outChar = 0;
	return 0;
}

int ImTextStrFromUtf8(ImWchar* buf, int bufSize, const char* inText, const char* inTextEnd, const char** inTextRemaining)
{
	ImWchar* bufOut = buf;
	ImWchar* bufEnd = buf + bufSize;
	while (bufOut < bufEnd - 1 && (!inTextEnd || inText < inTextEnd) && *inText)
	{
		unsigned int c;
		inText += ImTextCharFromUtf8(&c, inText, inTextEnd);
		if (c == 0)
			break;
		if (c < 0x10000)    //FIXME: Losing characters that don't fit in 2 bytes
			*bufOut++ = (ImWchar)c;
	}
	*bufOut = 0;
	if (inTextRemaining)
		*inTextRemaining = inText;
	return (int)(bufOut - buf);
}

int ImTextCountCharsFromUtf8(const char* inText, const char* inTextEnd)
{
	int charCount = 0;
	while ((!inTextEnd || inText < inTextEnd) && *inText)
	{
		unsigned int c;
		inText += ImTextCharFromUtf8(&c, inText, inTextEnd);
		if (c == 0)
			break;
		if (c < 0x10000)
			charCount++;
	}
	return charCount;
}

//Based on stb_to_utf8() from github.com/nothings/stb/
static inline int ImTextCharToUtf8(char* buf, int bufSize, unsigned int c)
{
	if (c < 0x80)
	{
		buf[0] = (char)c;
		return 1;
	}
	if (c < 0x800)
	{
		if (bufSize < 2)
			return 0;
		buf[0] = (char)(0xc0 + (c >> 6));
		buf[1] = (char)(0x80 + (c & 0x3f));
		return 2;
	}
	if (c >= 0xdc00 && c < 0xe000)
		return 0;
	if (c >= 0xd800 && c < 0xdc00)
	{
		if (bufSize < 4)
			return 0;
		buf[0] = (char)(0xf0 + (c >> 18));
		buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
		buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
		buf[3] = (char)(0x80 + ((c)& 0x3f));
		return 4;
	}
	//else if (c < 0x10000)
	{
		if (bufSize < 3)
			return 0;
		buf[0] = (char)(0xe0 + (c >> 12));
		buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
		buf[2] = (char)(0x80 + ((c)& 0x3f));
		return 3;
	}
}

static inline int ImTextCountUtf8BytesFromChar(unsigned int c)
{
	if (c < 0x80)
		return 1;
	if (c < 0x800)
		return 2;
	if (c >= 0xdc00 && c < 0xe000)
		return 0;
	if (c >= 0xd800 && c < 0xdc00)
		return 4;
	return 3;
}

int ImTextStrToUtf8(char* buf, int bufSize, const ImWchar* inText, const ImWchar* inTextEnd)
{
	char* bufOut = buf;
	const char* bufEnd = buf + bufSize;
	while (bufOut < bufEnd - 1 && (!inTextEnd || inText < inTextEnd) && *inText)
	{
		unsigned int c = (unsigned int)(*inText++);
		if (c < 0x80)
			*bufOut++ = (char)c;
		else
			bufOut += ImTextCharToUtf8(bufOut, (int)(bufEnd - bufOut - 1), c);
	}
	*bufOut = 0;
	return (int)(bufOut - buf);
}

int ImTextCountUtf8BytesFromStr(const ImWchar* inText, const ImWchar* inTextEnd)
{
	int bytesCount = 0;
	while ((!inTextEnd || inText < inTextEnd) && *inText)
	{
		unsigned int c = (unsigned int)(*inText++);
		if (c < 0x80)
			bytesCount++;
		else
			bytesCount += ImTextCountUtf8BytesFromChar(c);
	}
	return bytesCount;
}

ImVec4 ImGui::ColorConvertU32ToFloat4(ImU32 in)
{
	float s = 1.f / 255.f;
	return ImVec4((in & 0xFF) * s, ((in >> 8) & 0xFF) * s, ((in >> 16) & 0xFF) * s, (in >> 24) * s);
}

ImU32 ImGui::ColorConvertFloat4ToU32(const ImVec4& in)
{
	ImU32 out;
	out = ((ImU32)IM_F32_TO_INT8(ImSaturate(in.x)));
	out |= ((ImU32)IM_F32_TO_INT8(ImSaturate(in.y))) << 8;
	out |= ((ImU32)IM_F32_TO_INT8(ImSaturate(in.z))) << 16;
	out |= ((ImU32)IM_F32_TO_INT8(ImSaturate(in.w))) << 24;
	return out;
}

//Convert rgb floats ([0-1],[0-1],[0-1]) to hsv floats ([0-1],[0-1],[0-1]), from Foley & van Dam p592
//Optimized http://lolengine.net/blog/2013/01/13/fast-rgb-to-hsv
void ImGui::ColorConvertRGBtoHSV(float r, float g, float b, float& outH, float& outS, float& outV)
{
	float K = 0.f;
	if (g < b)
	{
		const float tmp = g;
		g = b;
		b = tmp;
		K = -1.f;
	}
	if (r < g)
	{
		const float tmp = r;
		r = g;
		g = tmp;
		K = -2.f / 6.f - K;
	}

	const float chroma = r - (g < b ? g : b);
	outH = fabsf(K + (g - b) / (6.f * chroma + 1e-20f));
	outS = chroma / (r + 1e-20f);
	outV = r;
}

//Convert hsv floats ([0-1],[0-1],[0-1]) to rgb floats ([0-1],[0-1],[0-1]), from Foley & van Dam p593
//also http://en.wikipedia.org/wiki/HSL_and_HSV
void ImGui::ColorConvertHSVtoRGB(float h, float s, float v, float& outR, float& outG, float& outB)
{
	if (s == 0.f)
	{
		//gray
		outR = outG = outB = v;
		return;
	}

	h = fmodf(h, 1.f) / (60.f / 360.f);
	int i = (int)h;
	float f = h - (float)i;
	float p = v * (1.f - s);
	float q = v * (1.f - s * f);
	float t = v * (1.f - s * (1.f - f));

	switch (i)
	{
	case 0:
		outR = v;
		outG = t;
		outB = p;
		break;
	case 1:
		outR = q;
		outG = v;
		outB = p;
		break;
	case 2:
		outR = p;
		outG = v;
		outB = t;
		break;
	case 3:
		outR = p;
		outG = q;
		outB = v;
		break;
	case 4:
		outR = t;
		outG = p;
		outB = v;
		break;
	case 5:
	default:
		outR = v;
		outG = p;
		outB = q;
		break;
	}
}

//Load file content into memory
//Memory allocated with ImGui::MemAlloc(), must be freed by user using ImGui::MemFree()
void* ImLoadFileToMemory(const char* filename, const char* fileOpenMode, int* outFileSize, int paddingBytes)
{
	IM_ASSERT(filename && fileOpenMode);
	if (outFileSize)
		*outFileSize = 0;

	FILE* f;
	if ((f = fopen(filename, fileOpenMode)) == NULL)
		return NULL;

	long fileSizeSigned;
	if (fseek(f, 0, SEEK_END) || (fileSizeSigned = ftell(f)) == -1 || fseek(f, 0, SEEK_SET))
	{
		fclose(f);
		return NULL;
	}

	int fileSize = (int)fileSizeSigned;
	void* fileData = ImGui::MemAlloc(fileSize + paddingBytes);
	if (fileData == NULL)
	{
		fclose(f);
		return NULL;
	}
	if (fread(fileData, 1, (size_t)fileSize, f) != (size_t)fileSize)
	{
		fclose(f);
		ImGui::MemFree(fileData);
		return NULL;
	}
	if (paddingBytes > 0)
		memset((void *)(((char*)fileData) + fileSize), 0, paddingBytes);

	fclose(f);
	if (outFileSize)
		*outFileSize = fileSize;

	return fileData;
}

//-----------------------------------------------------------------------------
//ImGuiStorage
//-----------------------------------------------------------------------------

//Helper: Key->value storage
void ImGuiStorage::Clear()
{
	Data.clear();
}

//std::lower_bound but without the bullshit
static ImVector<ImGuiStorage::Pair>::iterator LowerBound(ImVector<ImGuiStorage::Pair>& data, ImU32 key)
{
	ImVector<ImGuiStorage::Pair>::iterator first = data.begin();
	ImVector<ImGuiStorage::Pair>::iterator last = data.end();
	int count = (int)(last - first);
	while (count > 0)
	{
		int count2 = count / 2;
		ImVector<ImGuiStorage::Pair>::iterator mid = first + count2;
		if (mid->key < key)
		{
			first = ++mid;
			count -= count2 + 1;
		}
		else
			count = count2;
	}
	return first;
}

int ImGuiStorage::GetInt(ImU32 key, int defaultVal) const
{
	ImVector<Pair>::iterator it = LowerBound(const_cast<ImVector<ImGuiStorage::Pair>&>(Data), key);
	if (it == Data.end() || it->key != key)
		return defaultVal;
	return it->val_i;
}

float ImGuiStorage::GetFloat(ImU32 key, float defaultVal) const
{
	ImVector<Pair>::iterator it = LowerBound(const_cast<ImVector<ImGuiStorage::Pair>&>(Data), key);
	if (it == Data.end() || it->key != key)
		return defaultVal;
	return it->val_f;
}

void* ImGuiStorage::GetVoidPtr(ImGuiID key) const
{
	ImVector<Pair>::iterator it = LowerBound(const_cast<ImVector<ImGuiStorage::Pair>&>(Data), key);
	if (it == Data.end() || it->key != key)
		return NULL;
	return it->val_p;
}

//References are only valid until a new value is added to the storage. Calling a Set***() function or a Get***Ref() function invalidates the pointer.
int* ImGuiStorage::GetIntRef(ImGuiID key, int defaultVal)
{
	ImVector<Pair>::iterator it = LowerBound(Data, key);
	if (it == Data.end() || it->key != key)
		it = Data.insert(it, Pair(key, defaultVal));
	return &it->val_i;
}

float* ImGuiStorage::GetFloatRef(ImGuiID key, float defaultVal)
{
	ImVector<Pair>::iterator it = LowerBound(Data, key);
	if (it == Data.end() || it->key != key)
		it = Data.insert(it, Pair(key, defaultVal));
	return &it->val_f;
}

void** ImGuiStorage::GetVoidPtrRef(ImGuiID key, void* defaultVal)
{
	ImVector<Pair>::iterator it = LowerBound(Data, key);
	if (it == Data.end() || it->key != key)
		it = Data.insert(it, Pair(key, defaultVal));
	return &it->val_p;
}

//FIXME-OPT: Need a way to reuse the result of lower_bound when doing GetInt()/SetInt() - not too bad because it only happens on explicit interaction (maximum one a frame)
void ImGuiStorage::SetInt(ImU32 key, int val)
{
	ImVector<Pair>::iterator it = LowerBound(Data, key);
	if (it == Data.end() || it->key != key)
	{
		Data.insert(it, Pair(key, val));
		return;
	}
	it->val_i = val;
}

void ImGuiStorage::SetFloat(ImU32 key, float val)
{
	ImVector<Pair>::iterator it = LowerBound(Data, key);
	if (it == Data.end() || it->key != key)
	{
		Data.insert(it, Pair(key, val));
		return;
	}
	it->val_f = val;
}

void ImGuiStorage::SetVoidPtr(ImU32 key, void* val)
{
	ImVector<Pair>::iterator it = LowerBound(Data, key);
	if (it == Data.end() || it->key != key)
	{
		Data.insert(it, Pair(key, val));
		return;
	}
	it->val_p = val;
}

void ImGuiStorage::SetAllInt(int v)
{
	for (int i = 0; i < Data.Size; i++)
		Data[i].val_i = v;
}

//-----------------------------------------------------------------------------
//ImGuiTextFilter
//-----------------------------------------------------------------------------

//Helper: Parse and apply text filters. In format "aaaaa[,bbbb][,ccccc]"
ImGuiTextFilter::ImGuiTextFilter(const char* defaultFilter)
{
	if (defaultFilter)
	{
		ImFormatString(InputBuf, IM_ARRAYSIZE(InputBuf), "%s", defaultFilter);
		Build();
	}
	else
	{
		InputBuf[0] = 0;
		CountGrep = 0;
	}
}

bool ImGuiTextFilter::Draw(const char* label, float width)
{
	if (width != 0.f)
		ImGui::PushItemWidth(width);
	bool valueChanged = ImGui::InputText(label, InputBuf, IM_ARRAYSIZE(InputBuf));
	if (width != 0.f)
		ImGui::PopItemWidth();
	if (valueChanged)
		Build();
	return valueChanged;
}

void ImGuiTextFilter::TextRange::split(char separator, ImVector<TextRange>& out)
{
	out.resize(0);
	const char* wb = b;
	const char* we = wb;
	while (we < e)
	{
		if (*we == separator)
		{
			out.push_back(TextRange(wb, we));
			wb = we + 1;
		}
		we++;
	}
	if (wb != we)
		out.push_back(TextRange(wb, we));
}

void ImGuiTextFilter::Build()
{
	Filters.resize(0);
	TextRange inputRange(InputBuf, InputBuf + strlen(InputBuf));
	inputRange.split(',', Filters);

	CountGrep = 0;
	for (int i = 0; i != Filters.Size; i++)
	{
		Filters[i].trim_blanks();
		if (Filters[i].empty())
			continue;
		if (Filters[i].front() != '-')
			CountGrep += 1;
	}
}

bool ImGuiTextFilter::PassFilter(const char* text, const char* textEnd) const
{
	if (Filters.empty())
		return true;

	if (text == NULL)
		text = "";

	for (int i = 0; i != Filters.Size; i++)
	{
		const TextRange& f = Filters[i];
		if (f.empty())
			continue;
		if (f.front() == '-')
		{
			//Subtract
			if (ImStristr(text, textEnd, f.begin() + 1, f.end()) != NULL)
				return false;
		}
		else
		{
			//Grep
			if (ImStristr(text, textEnd, f.begin(), f.end()) != NULL)
				return true;
		}
	}

	//Implicit * grep
	if (CountGrep == 0)
		return true;

	return false;
}

//-----------------------------------------------------------------------------
//ImGuiTextBuffer
//-----------------------------------------------------------------------------

//On some platform vsnprintf() takes va_list by reference and modifies it.
//va_copy is the 'correct' way to copy a va_list but Visual Studio prior to 2013 doesn't have it.
#ifndef va_copy
#define va_copy(dest, src) (dest = src)
#endif

//Helper: Text buffer for logging/accumulating text
void ImGuiTextBuffer::appendv(const char* fmt, va_list args)
{
	va_list argsCopy;
	va_copy(argsCopy, args);

	int len = vsnprintf(NULL, 0, fmt, args);	//FIXME-OPT: could do a first pass write attempt, likely successful on first pass.
	if (len <= 0)
		return;

	const int writeOff = Buf.Size;
	const int neededSz = writeOff + len;
	if (writeOff + len >= Buf.Capacity)
	{
		int doubleCapacity = Buf.Capacity * 2;
		Buf.reserve(neededSz > doubleCapacity ? neededSz : doubleCapacity);
	}

	Buf.resize(neededSz);
	ImFormatStringV(&Buf[writeOff] - 1, len + 1, fmt, argsCopy);
}

void ImGuiTextBuffer::append(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	appendv(fmt, args);
	va_end(args);
}

//-----------------------------------------------------------------------------
//ImGuiSimpleColumns
//-----------------------------------------------------------------------------

ImGuiSimpleColumns::ImGuiSimpleColumns()
{
	Count = 0;
	Spacing = Width = NextWidth = 0.f;
	memset(Pos, 0, sizeof(Pos));
	memset(NextWidths, 0, sizeof(NextWidths));
}

void ImGuiSimpleColumns::Update(int count, float spacing, bool clear)
{
	IM_ASSERT(Count <= IM_ARRAYSIZE(Pos));
	Count = count;
	Width = NextWidth = 0.f;
	Spacing = spacing;
	if (clear)
		memset(NextWidths, 0, sizeof(NextWidths));
	for (int i = 0; i < Count; i++)
	{
		if (i > 0 && NextWidths[i] > 0.f)
			Width += Spacing;
		Pos[i] = (float)(int)Width;
		Width += NextWidths[i];
		NextWidths[i] = 0.f;
	}
}

float ImGuiSimpleColumns::DeclColumns(float w0, float w1, float w2)	//not using va_arg because they promote float to double
{
	NextWidth = 0.f;
	NextWidths[0] = ImMax(NextWidths[0], w0);
	NextWidths[1] = ImMax(NextWidths[1], w1);
	NextWidths[2] = ImMax(NextWidths[2], w2);
	for (int i = 0; i < 3; i++)
		NextWidth += NextWidths[i] + ((i > 0 && NextWidths[i] > 0.f) ? Spacing : 0.f);
	return ImMax(Width, NextWidth);
}

float ImGuiSimpleColumns::CalcExtraSpace(float availW)
{
	return ImMax(0.f, availW - Width);
}

//-----------------------------------------------------------------------------
//ImGuiWindow
//-----------------------------------------------------------------------------

ImGuiWindow::ImGuiWindow(const char* name)
{
	Name = ImStrdup(name);
	ID = ImHash(name, 0);
	IDStack.push_back(ID);
	MoveID = GetID("#MOVE");

	Flags = 0;
	PosFloat = Pos = ImVec2(0.f, 0.f);
	Size = SizeFull = ImVec2(0.f, 0.f);
	SizeContents = SizeContentsExplicit = ImVec2(0.f, 0.f);
	WindowPadding = ImVec2(0.f, 0.f);
	Scroll = ImVec2(0.f, 0.f);
	ScrollTarget = ImVec2(FLT_MAX, FLT_MAX);
	ScrollTargetCenterRatio = ImVec2(0.5f, 0.5f);
	ScrollbarX = ScrollbarY = false;
	ScrollbarSizes = ImVec2(0.f, 0.f);
	Active = WasActive = false;
	Accessed = false;
	Collapsed = false;
	SkipItems = false;
	BeginCount = 0;
	PopupID = 0;
	AutoFitFramesX = AutoFitFramesY = -1;
	AutoFitOnlyGrows = false;
	AutoPosLastDirection = -1;
	HiddenFrames = 0;
	SetWindowPosAllowFlags = SetWindowSizeAllowFlags = SetWindowCollapsedAllowFlags = ImGuiSetCond_Always | ImGuiSetCond_Once | ImGuiSetCond_FirstUseEver | ImGuiSetCond_Appearing;
	SetWindowPosCenterWanted = false;

	LastFrameActive = -1;
	ItemWidthDefault = 0.f;
	FontWindowScale = 1.f;

	DrawList = (ImDrawList*)ImGui::MemAlloc(sizeof(ImDrawList));
	new(DrawList) ImDrawList();
	DrawList->_OwnerName = Name;
	RootWindow = NULL;
	RootNonPopupWindow = NULL;

	FocusIdxAllCounter = FocusIdxTabCounter = -1;
	FocusIdxAllRequestCurrent = FocusIdxTabRequestCurrent = IM_INT_MAX;
	FocusIdxAllRequestNext = FocusIdxTabRequestNext = IM_INT_MAX;
}

ImGuiWindow::~ImGuiWindow()
{
	DrawList->~ImDrawList();
	ImGui::MemFree(DrawList);
	DrawList = NULL;
	ImGui::MemFree(Name);
	Name = NULL;
}

ImGuiID ImGuiWindow::GetID(const char* str, const char* strEnd)
{
	ImGuiID seed = IDStack.back();
	ImGuiID id = ImHash(str, strEnd ? (int)(strEnd - str) : 0, seed);
	ImGui::KeepAliveID(id);
	return id;
}

ImGuiID ImGuiWindow::GetID(const void* ptr)
{
	ImGuiID seed = IDStack.back();
	ImGuiID id = ImHash(&ptr, sizeof(void*), seed);
	ImGui::KeepAliveID(id);
	return id;
}

//-----------------------------------------------------------------------------
//Internal API exposed in imgui_internal.h
//-----------------------------------------------------------------------------

static void SetCurrentWindow(ImGuiWindow* window)
{
	ImGuiState& g = *GImGui;
	g.CurrentWindow = window;
	if (window)
		g.FontSize = window->CalcFontSize();
}

ImGuiWindow* ImGui::GetParentWindow()
{
	ImGuiState& g = *GImGui;
	IM_ASSERT(g.CurrentWindowStack.Size >= 2);
	return g.CurrentWindowStack[g.CurrentWindowStack.Size - 2];
}

void ImGui::SetActiveID(ImGuiID id, ImGuiWindow* window = NULL)
{
	ImGuiState& g = *GImGui;
	g.ActiveId = id;
	g.ActiveIdAllowHoveringOthers = false;
	g.ActiveIdIsJustActivated = true;
	g.ActiveIdWindow = window;
}

void ImGui::SetHoveredID(ImGuiID id)
{
	ImGuiState& g = *GImGui;
	g.HoveredId = id;
	g.HoveredIdAllowHoveringOthers = false;
}

void ImGui::KeepAliveID(ImGuiID id)
{
	ImGuiState& g = *GImGui;
	if (g.ActiveId == id)
		g.ActiveIdIsAlive = true;
}

//Advance cursor given item size for layout.
void ImGui::ItemSize(const ImVec2& size, float textOffsetY)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	//Always align ourselves on pixel boundaries
	ImGuiState& g = *GImGui;
	const float lineHeight = ImMax(window->DC.CurrentLineHeight, size.y);
	const float textBaseOffset = ImMax(window->DC.CurrentLineTextBaseOffset, textOffsetY);
	window->DC.CursorPosPrevLine = ImVec2(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y);
	window->DC.CursorPos = ImVec2((float)(int)(window->Pos.x + window->DC.ColumnsStartX + window->DC.ColumnsOffsetX), (float)(int)(window->DC.CursorPos.y + lineHeight + g.Style.ItemSpacing.y));
	window->DC.CursorMaxPos.x = ImMax(window->DC.CursorMaxPos.x, window->DC.CursorPosPrevLine.x);
	window->DC.CursorMaxPos.y = ImMax(window->DC.CursorMaxPos.y, window->DC.CursorPos.y);

	//window->DrawList->AddCircle(window->DC.CursorMaxPos, 3.f, 0xFF0000FF, 4); //Debug

	window->DC.PrevLineHeight = lineHeight;
	window->DC.PrevLineTextBaseOffset = textBaseOffset;
	window->DC.CurrentLineHeight = window->DC.CurrentLineTextBaseOffset = 0.f;
}

void ImGui::ItemSize(const ImRect& bb, float textOffsetY)
{
	ItemSize(bb.GetSize(), textOffsetY);
}

//Declare item bounding box for clipping and interaction.
//Note that the size can be different than the one provided to ItemSize(). Typically, widgets that spread over available surface
//declares their minimum size requirement to ItemSize() and then use a larger region for drawing/interaction, which is passed to ItemAdd().
bool ImGui::ItemAdd(const ImRect& bb, const ImGuiID* id)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.LastItemID = id ? *id : 0;
	window->DC.LastItemRect = bb;
	if (IsClippedEx(bb, id, false))
	{
		window->DC.LastItemHoveredAndUsable = window->DC.LastItemHoveredRect = false;
		return false;
	}

	//This is a sensible default, but widgets are free to override it after calling ItemAdd()
	ImGuiState& g = *GImGui;
	if (IsMouseHoveringRect(bb.Min, bb.Max))
	{
		//Matching the behavior of IsHovered() but ignore if ActiveId==window->MoveID (we clicked on the window background)
		//So that clicking on items with no active id such as Text() still returns true with IsItemHovered()
		window->DC.LastItemHoveredRect = true;
		window->DC.LastItemHoveredAndUsable = false;
		if (g.HoveredRootWindow == window->RootWindow)
		{
			if (g.ActiveId == 0 || (id && g.ActiveId == *id) || g.ActiveIdAllowHoveringOthers || (g.ActiveId == window->MoveID))
			{
				if (IsWindowContentHoverable(window))
					window->DC.LastItemHoveredAndUsable = true;
			}
		}
	}
	else
		window->DC.LastItemHoveredAndUsable = window->DC.LastItemHoveredRect = false;

	return true;
}

bool ImGui::IsClippedEx(const ImRect& bb, const ImGuiID* id, bool clipEvenWhenLogged)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindowRead();

	if (!bb.Overlaps(window->ClipRect))
	{
		if (!id || *id != GImGui->ActiveId)
		{
			if (clipEvenWhenLogged || !g.LogEnabled)
				return true;
		}
	}
	return false;
}

bool ImGui::IsHovered(const ImRect& bb, ImGuiID id, bool flattenChilds)
{
	ImGuiState& g = *GImGui;
	if (g.HoveredId == 0 || g.HoveredId == id || g.HoveredIdAllowHoveringOthers)
	{
		ImGuiWindow* window = GetCurrentWindowRead();
		if (g.HoveredWindow == window || (flattenChilds && g.HoveredRootWindow == window->RootWindow))
		{
			if ((g.ActiveId == 0 || g.ActiveId == id || g.ActiveIdAllowHoveringOthers) && ImGui::IsMouseHoveringRect(bb.Min, bb.Max))
			{
				if (IsWindowContentHoverable(g.HoveredRootWindow))
					return true;
			}
		}
	}
	return false;
}

bool ImGui::FocusableItemRegister(ImGuiWindow* window, bool isActive, bool tabStop)
{
	ImGuiState& g = *GImGui;

	const bool allowKeyboardFocus = window->DC.AllowKeyboardFocus;
	window->FocusIdxAllCounter++;
	if (allowKeyboardFocus)
		window->FocusIdxTabCounter++;

	//Process keyboard input at this point: TAB, Shift-TAB switch focus
	//We can always TAB out of a widget that doesn't allow tabbing in.
	if (tabStop && window->FocusIdxAllRequestNext == IM_INT_MAX && window->FocusIdxTabRequestNext == IM_INT_MAX && isActive && IsKeyPressedMap(ImGuiKey_Tab))
	{
		//Modulo on index will be applied at the end of frame once we've got the total counter of items.
		window->FocusIdxTabRequestNext = window->FocusIdxTabCounter + (g.IO.KeyShift ? (allowKeyboardFocus ? -1 : 0) : +1);
	}

	if (window->FocusIdxAllCounter == window->FocusIdxAllRequestCurrent)
		return true;

	if (allowKeyboardFocus)
	{
		if (window->FocusIdxTabCounter == window->FocusIdxTabRequestCurrent)
			return true;
	}

	return false;
}

void ImGui::FocusableItemUnregister(ImGuiWindow* window)
{
	window->FocusIdxAllCounter--;
	window->FocusIdxTabCounter--;
}

ImVec2 ImGui::CalcItemSize(ImVec2 size, float defaultX, float defaultY)
{
	ImGuiState& g = *GImGui;
	ImVec2 contentMax;
	if (size.x < 0.f || size.y < 0.f)
		contentMax = g.CurrentWindow->Pos + ImGui::GetContentRegionMax();
	if (size.x <= 0.f)
		size.x = (size.x == 0.f) ? defaultX : ImMax(contentMax.x - g.CurrentWindow->DC.CursorPos.x, 4.f) + size.x;
	if (size.y <= 0.f)
		size.y = (size.y == 0.f) ? defaultY : ImMax(contentMax.y - g.CurrentWindow->DC.CursorPos.y, 4.f) + size.y;
	return size;
}

float ImGui::CalcWrapWidthForPos(const ImVec2& pos, float wrapPosX)
{
	if (wrapPosX < 0.f)
		return 0.f;

	ImGuiWindow* window = GetCurrentWindowRead();
	if (wrapPosX == 0.f)
		wrapPosX = ImGui::GetContentRegionMax().x + window->Pos.x;
	else if (wrapPosX > 0.f)
		wrapPosX += window->Pos.x - window->Scroll.x;	//wrapPosX is provided is window local space

	const float wrapWidth = wrapPosX > 0.f ? ImMax(wrapPosX - pos.x, 0.00001f) : 0.f;
	return wrapWidth;
}

//-----------------------------------------------------------------------------

void* ImGui::MemAlloc(size_t sz)
{
	GImGui->IO.MetricsAllocs++;
	return GImGui->IO.MemAllocFn(sz);
}

void ImGui::MemFree(void* ptr)
{
	if (ptr)
		GImGui->IO.MetricsAllocs--;
	return GImGui->IO.MemFreeFn(ptr);
}

const char* ImGui::GetClipboardText()
{
	return GImGui->IO.GetClipboardTextFn ? GImGui->IO.GetClipboardTextFn() : "";
}

void ImGui::SetClipboardText(const char* text)
{
	if (GImGui->IO.SetClipboardTextFn)
		GImGui->IO.SetClipboardTextFn(text);
}

const char* ImGui::GetVersion()
{
	return IMGUI_VERSION;
}

//Internal state access - if you want to share ImGui state between modules (e.g. DLL) or allocate it yourself
//Note that we still point to some static data and members (such as GFontAtlas), so the state instance you end up using will point to the static data within its module
void* ImGui::GetInternalState()
{
	return GImGui;
}

size_t ImGui::GetInternalStateSize()
{
	return sizeof(ImGuiState);
}

void ImGui::SetInternalState(void* state, bool construct)
{
	if (construct)
		new (state) ImGuiState();
	GImGui = (ImGuiState*)state;
}

ImGuiIO& ImGui::GetIO()
{
	return GImGui->IO;
}

ImGuiStyle& ImGui::GetStyle()
{
	return GImGui->Style;
}

//Same value as passed to your RenderDrawListsFn() function. valid after Render() and until the next call to NewFrame()
ImDrawData* ImGui::GetDrawData()
{
	return GImGui->RenderDrawData.Valid ? &GImGui->RenderDrawData : NULL;
}

float ImGui::GetTime()
{
	return GImGui->Time;
}

int ImGui::GetFrameCount()
{
	return GImGui->FrameCount;
}

void ImGui::NewFrame()
{
	ImGuiState& g = *GImGui;

	//Check user data
	IM_ASSERT(g.IO.DeltaTime >= 0.f);
	IM_ASSERT(g.IO.DisplaySize.x >= 0.f && g.IO.DisplaySize.y >= 0.f);
	IM_ASSERT(g.IO.Fonts->Fonts.Size > 0);	//Font Atlas not created. Did you call io.Fonts->GetTexDataAsRGBA32 / GetTexDataAsAlpha8 ?
	IM_ASSERT(g.IO.Fonts->Fonts[0]->IsLoaded());	//Font Atlas not created. Did you call io.Fonts->GetTexDataAsRGBA32 / GetTexDataAsAlpha8 ?
	IM_ASSERT(g.Style.CurveTessellationTol > 0.f);	//Invalid

	if (!g.Initialized)
	{
		//Initialize on first frame
		g.LogClipboard = (ImGuiTextBuffer*)ImGui::MemAlloc(sizeof(ImGuiTextBuffer));
		new(g.LogClipboard) ImGuiTextBuffer();

		IM_ASSERT(g.Settings.empty());
		LoadSettings();
		g.Initialized = true;
	}

	SetCurrentFont(g.IO.Fonts->Fonts[0]);

	g.Time += g.IO.DeltaTime;
	g.FrameCount += 1;
	g.Tooltip[0] = '\0';
	g.OverlayDrawList.Clear();
	g.OverlayDrawList.PushTextureID(g.IO.Fonts->TexID);
	g.OverlayDrawList.PushClipRectFullScreen();
	g.OverlayDrawList.AddDrawCmd();

	//Mark rendering data as invalid to prevent user who may have a handle on it to use it
	g.RenderDrawData.Valid = false;
	g.RenderDrawData.CmdLists = NULL;
	g.RenderDrawData.CmdListsCount = g.RenderDrawData.TotalVtxCount = g.RenderDrawData.TotalIdxCount = 0;

	//Update inputs state
	if (g.IO.MousePos.x < 0 && g.IO.MousePos.y < 0)
		g.IO.MousePos = ImVec2(-9999.f, -9999.f);
	if ((g.IO.MousePos.x < 0 && g.IO.MousePos.y < 0) || (g.IO.MousePosPrev.x < 0 && g.IO.MousePosPrev.y < 0))	//if mouse just appeared or disappeared (negative coordinate) we cancel out movement in MouseDelta
		g.IO.MouseDelta = ImVec2(0.f, 0.f);
	else
		g.IO.MouseDelta = g.IO.MousePos - g.IO.MousePosPrev;
	g.IO.MousePosPrev = g.IO.MousePos;
	for (int i = 0; i < IM_ARRAYSIZE(g.IO.MouseDown); i++)
	{
		g.IO.MouseClicked[i] = g.IO.MouseDown[i] && g.IO.MouseDownDuration[i] < 0.f;
		g.IO.MouseReleased[i] = !g.IO.MouseDown[i] && g.IO.MouseDownDuration[i] >= 0.f;
		g.IO.MouseDownDurationPrev[i] = g.IO.MouseDownDuration[i];
		g.IO.MouseDownDuration[i] = g.IO.MouseDown[i] ? (g.IO.MouseDownDuration[i] < 0.f ? 0.f : g.IO.MouseDownDuration[i] + g.IO.DeltaTime) : -1.f;
		g.IO.MouseDoubleClicked[i] = false;
		if (g.IO.MouseClicked[i])
		{
			if (g.Time - g.IO.MouseClickedTime[i] < g.IO.MouseDoubleClickTime)
			{
				if (ImLengthSqr(g.IO.MousePos - g.IO.MouseClickedPos[i]) < g.IO.MouseDoubleClickMaxDist * g.IO.MouseDoubleClickMaxDist)
					g.IO.MouseDoubleClicked[i] = true;
				g.IO.MouseClickedTime[i] = -FLT_MAX;	//so the third click isn't turned into a double-click
			}
			else
				g.IO.MouseClickedTime[i] = g.Time;
			g.IO.MouseClickedPos[i] = g.IO.MousePos;
			g.IO.MouseDragMaxDistanceSqr[i] = 0.f;
		}
		else if (g.IO.MouseDown[i])
			g.IO.MouseDragMaxDistanceSqr[i] = ImMax(g.IO.MouseDragMaxDistanceSqr[i], ImLengthSqr(g.IO.MousePos - g.IO.MouseClickedPos[i]));
	}
	memcpy(g.IO.KeysDownDurationPrev, g.IO.KeysDownDuration, sizeof(g.IO.KeysDownDuration));
	for (int i = 0; i < IM_ARRAYSIZE(g.IO.KeysDown); i++)
		g.IO.KeysDownDuration[i] = g.IO.KeysDown[i] ? (g.IO.KeysDownDuration[i] < 0.f ? 0.f : g.IO.KeysDownDuration[i] + g.IO.DeltaTime) : -1.f;

	//Calculate frame-rate for the user, as a purely luxurious feature
	g.FramerateSecPerFrameAccum += g.IO.DeltaTime - g.FramerateSecPerFrame[g.FramerateSecPerFrameIdx];
	g.FramerateSecPerFrame[g.FramerateSecPerFrameIdx] = g.IO.DeltaTime;
	g.FramerateSecPerFrameIdx = (g.FramerateSecPerFrameIdx + 1) % IM_ARRAYSIZE(g.FramerateSecPerFrame);
	g.IO.Framerate = 1.f / (g.FramerateSecPerFrameAccum / (float)IM_ARRAYSIZE(g.FramerateSecPerFrame));

	//Clear reference to active widget if the widget isn't alive anymore
	g.HoveredIdPreviousFrame = g.HoveredId;
	g.HoveredId = 0;
	g.HoveredIdAllowHoveringOthers = false;
	if (!g.ActiveIdIsAlive && g.ActiveIdPreviousFrame == g.ActiveId && g.ActiveId != 0)
		SetActiveID(0);
	g.ActiveIdPreviousFrame = g.ActiveId;
	g.ActiveIdIsAlive = false;
	g.ActiveIdIsJustActivated = false;
	if (!g.ActiveId)
		g.MovedWindow = NULL;

	//Delay saving settings so we don't spam disk too much
	if (g.SettingsDirtyTimer > 0.f)
	{
		g.SettingsDirtyTimer -= g.IO.DeltaTime;
		if (g.SettingsDirtyTimer <= 0.f)
			SaveSettings();
	}

	//Find the window we are hovering. Child windows can extend beyond the limit of their parent so we need to derive HoveredRootWindow from HoveredWindow
	g.HoveredWindow = FindHoveredWindow(g.IO.MousePos, false);
	if (g.HoveredWindow && (g.HoveredWindow->Flags & ImGuiWindowFlags_ChildWindow))
		g.HoveredRootWindow = g.HoveredWindow->RootWindow;
	else
		g.HoveredRootWindow = FindHoveredWindow(g.IO.MousePos, true);

	if (ImGuiWindow* modalWindow = GetFrontMostModalRootWindow())
	{
		g.ModalWindowDarkeningRatio = ImMin(g.ModalWindowDarkeningRatio + g.IO.DeltaTime * 6.f, 1.f);
		if (g.HoveredRootWindow != modalWindow)
			g.HoveredRootWindow = g.HoveredWindow = NULL;
	}
	else
		g.ModalWindowDarkeningRatio = 0.f;

	//Are we using inputs? Tell user so they can capture/discard the inputs away from the rest of their application.
	//When clicking outside of a window we assume the click is owned by the application and won't request capture.
	int mouseEarliestButtonDown = -1;
	bool mouseAnyDown = false;
	for (int i = 0; i < IM_ARRAYSIZE(g.IO.MouseDown); i++)
	{
		if (g.IO.MouseClicked[i])
			g.IO.MouseDownOwned[i] = (g.HoveredWindow != NULL) || (!g.OpenedPopupStack.empty());
		mouseAnyDown |= g.IO.MouseDown[i];
		if (g.IO.MouseDown[i])
		{
			if (mouseEarliestButtonDown == -1 || g.IO.MouseClickedTime[mouseEarliestButtonDown] > g.IO.MouseClickedTime[i])
				mouseEarliestButtonDown = i;
		}
	}
	bool mouseOwnedByApplication = mouseEarliestButtonDown != -1 && !g.IO.MouseDownOwned[mouseEarliestButtonDown];
	g.IO.WantCaptureMouse = (!mouseOwnedByApplication && g.HoveredWindow != NULL) || (!mouseOwnedByApplication && mouseAnyDown) || (g.ActiveId != 0) || (!g.OpenedPopupStack.empty()) || 
								(g.CaptureMouseNextFrame);
	g.IO.WantCaptureKeyboard = (g.ActiveId != 0) || (g.CaptureKeyboardNextFrame);
	g.IO.WantTextInput = (g.ActiveId != 0 && g.InputTextState.Id == g.ActiveId);
	g.MouseCursor = ImGuiMouseCursor_Arrow;
	g.CaptureMouseNextFrame = g.CaptureKeyboardNextFrame = false;

	//If mouse was first clicked outside of ImGui bounds we also cancel out hovering.
	if (mouseOwnedByApplication)
		g.HoveredWindow = g.HoveredRootWindow = NULL;

	//Scale & Scrolling
	if (g.HoveredWindow && g.IO.MouseWheel != 0.f && !g.HoveredWindow->Collapsed)
	{
		ImGuiWindow* window = g.HoveredWindow;
		if (g.IO.KeyCtrl)
		{
			if (g.IO.FontAllowUserScaling)
			{
				//Zoom / Scale window
				float newFontScale = ImClamp(window->FontWindowScale + g.IO.MouseWheel * 0.1f, 0.5f, 2.5f);
				float scale = newFontScale / window->FontWindowScale;
				window->FontWindowScale = newFontScale;

				const ImVec2 offset = window->Size * (1.f - scale) * (g.IO.MousePos - window->Pos) / window->Size;
				window->Pos += offset;
				window->PosFloat += offset;
				window->Size *= scale;
				window->SizeFull *= scale;
			}
		}
		else
		{
			//Scroll
			if (!(window->Flags & ImGuiWindowFlags_NoScrollWithMouse))
			{
				const int scrollLines = (window->Flags & ImGuiWindowFlags_ComboBox) ? 3 : 5;
				SetWindowScrollY(window, window->Scroll.y - g.IO.MouseWheel * window->CalcFontSize() * scrollLines);
			}
		}
	}

	//Pressing TAB activate widget focus
	//NB: Don't discard FocusedWindow if it isn't active, so that a window that go on/off programatically won't lose its keyboard focus.
	if (g.ActiveId == 0 && g.FocusedWindow != NULL && g.FocusedWindow->Active && IsKeyPressedMap(ImGuiKey_Tab, false))
		g.FocusedWindow->FocusIdxTabRequestNext = 0;

	//Mark all windows as not visible
	for (int i = 0; i != g.Windows.Size; i++)
	{
		ImGuiWindow* window = g.Windows[i];
		window->WasActive = window->Active;
		window->Active = false;
		window->Accessed = false;
	}

	//No window should be open at the beginning of the frame.
	//But in order to allow the user to call NewFrame() multiple times without calling Render(), we are doing an explicit clear.
	g.CurrentWindowStack.resize(0);
	g.CurrentPopupStack.resize(0);
	CloseInactivePopups();

	//Create implicit window - we will only render it if the user has added something to it.
	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Debug");
}

//NB: behavior of ImGui after Shutdown() is not tested/guaranteed at the moment. This function is merely here to free heap allocations.
void ImGui::Shutdown()
{
	ImGuiState& g = *GImGui;

	//The fonts atlas can be used prior to calling NewFrame(), so we clear it even if g.Initialized is FALSE (which would happen if we never called NewFrame)
	if (g.IO.Fonts) //Testing for NULL to allow user to NULLify in case of running Shutdown() on multiple contexts. Bit hacky.
		g.IO.Fonts->Clear();

	//Cleanup of other data are conditional on actually having used ImGui.
	if (!g.Initialized)
		return;

	SaveSettings();

	for (int i = 0; i < g.Windows.Size; i++)
	{
		g.Windows[i]->~ImGuiWindow();
		ImGui::MemFree(g.Windows[i]);
	}
	g.Windows.clear();
	g.WindowsSortBuffer.clear();
	g.CurrentWindowStack.clear();
	g.FocusedWindow = NULL;
	g.HoveredWindow = NULL;
	g.HoveredRootWindow = NULL;
	for (int i = 0; i < g.Settings.Size; i++)
		ImGui::MemFree(g.Settings[i].Name);
	g.Settings.clear();
	g.ColorModifiers.clear();
	g.StyleModifiers.clear();
	g.FontStack.clear();
	g.OpenedPopupStack.clear();
	g.CurrentPopupStack.clear();
	for (int i = 0; i < IM_ARRAYSIZE(g.RenderDrawLists); i++)
		g.RenderDrawLists[i].clear();
	g.OverlayDrawList.ClearFreeMemory();
	g.ColorEditModeStorage.Clear();
	if (g.PrivateClipboard)
	{
		ImGui::MemFree(g.PrivateClipboard);
		g.PrivateClipboard = NULL;
	}
	g.InputTextState.Text.clear();
	g.InputTextState.InitialText.clear();
	g.InputTextState.TempTextBuffer.clear();

	if (g.LogFile && g.LogFile != stdout)
	{
		fclose(g.LogFile);
		g.LogFile = NULL;
	}
	if (g.LogClipboard)
	{
		g.LogClipboard->~ImGuiTextBuffer();
		ImGui::MemFree(g.LogClipboard);
	}

	g.Initialized = false;
}

static ImGuiIniData* FindWindowSettings(const char* name)
{
	ImGuiState& g = *GImGui;
	ImGuiID id = ImHash(name, 0);
	for (int i = 0; i != g.Settings.Size; i++)
	{
		ImGuiIniData* ini = &g.Settings[i];
		if (ini->ID == id)
			return ini;
	}
	return NULL;
}

static ImGuiIniData* AddWindowSettings(const char* name)
{
	GImGui->Settings.resize(GImGui->Settings.Size + 1);
	ImGuiIniData* ini = &GImGui->Settings.back();
	ini->Name = ImStrdup(name);
	ini->ID = ImHash(name, 0);
	ini->Collapsed = false;
	ini->Pos = ImVec2(FLT_MAX, FLT_MAX);
	ini->Size = ImVec2(0, 0);
	return ini;
}

//Zero-tolerance, poor-man .ini parsing
//FIXME: Write something less rubbish
static void LoadSettings()
{
	ImGuiState& g = *GImGui;
	const char* filename = g.IO.IniFilename;
	if (!filename)
		return;

	int fileSize;
	char* fileData = (char*)ImLoadFileToMemory(filename, "rb", &fileSize, 1);
	if (!fileData)
		return;

	ImGuiIniData* settings = NULL;
	const char* bufEnd = fileData + fileSize;
	for (const char* lineStart = fileData; lineStart < bufEnd; )
	{
		const char* lineEnd = lineStart;
		while (lineEnd < bufEnd && *lineEnd != '\n' && *lineEnd != '\r')
			lineEnd++;

		if (lineStart[0] == '[' && lineEnd > lineStart && lineEnd[-1] == ']')
		{
			char name[64];
			ImFormatString(name, IM_ARRAYSIZE(name), "%.*s", (int)(lineEnd - lineStart - 2), lineStart + 1);
			settings = FindWindowSettings(name);
			if (!settings)
				settings = AddWindowSettings(name);
		}
		else if (settings)
		{
			float x;
			float y;
			int i;
			if (sscanf(lineStart, "Pos=%f,%f", &x, &y) == 2)
				settings->Pos = ImVec2(x, y);
			else if (sscanf(lineStart, "Size=%f,%f", &x, &y) == 2)
				settings->Size = ImMax(ImVec2(x, y), g.Style.WindowMinSize);
			else if (sscanf(lineStart, "Collapsed=%d", &i) == 1)
				settings->Collapsed = (i != 0);
		}

		lineStart = lineEnd + 1;
	}

	ImGui::MemFree(fileData);
}

static void SaveSettings()
{
	ImGuiState& g = *GImGui;
	const char* filename = g.IO.IniFilename;
	if (!filename)
		return;

	//Gather data from windows that were active during this session
	for (int i = 0; i != g.Windows.Size; i++)
	{
		ImGuiWindow* window = g.Windows[i];
		if (window->Flags & ImGuiWindowFlags_NoSavedSettings)
			continue;
		ImGuiIniData* settings = FindWindowSettings(window->Name);
		settings->Pos = window->Pos;
		settings->Size = window->SizeFull;
		settings->Collapsed = window->Collapsed;
	}

	//Write .ini file
	//If a window wasn't opened in this session we preserve its settings
	FILE* f = fopen(filename, "wt");
	if (!f)
		return;
	for (int i = 0; i != g.Settings.Size; i++)
	{
		const ImGuiIniData* settings = &g.Settings[i];
		if (settings->Pos.x == FLT_MAX)
			continue;
		const char* name = settings->Name;
		if (const char* p = strstr(name, "###"))  //Skip to the "###" marker if any. We don't skip past to match the behavior of GetID()
			name = p;
		fprintf(f, "[%s]\n", name);
		fprintf(f, "Pos=%d,%d\n", (int)settings->Pos.x, (int)settings->Pos.y);
		fprintf(f, "Size=%d,%d\n", (int)settings->Size.x, (int)settings->Size.y);
		fprintf(f, "Collapsed=%d\n", settings->Collapsed);
		fprintf(f, "\n");
	}

	fclose(f);
}

static void MarkSettingsDirty()
{
	ImGuiState& g = *GImGui;
	if (g.SettingsDirtyTimer <= 0.f)
		g.SettingsDirtyTimer = g.IO.IniSavingRate;
}

//FIXME: Add a more explicit sort order in the window structure.
static int ChildWindowComparer(const void* lhs, const void* rhs)
{
	const ImGuiWindow* a = *(const ImGuiWindow**)lhs;
	const ImGuiWindow* b = *(const ImGuiWindow**)rhs;
	if (int d = (a->Flags & ImGuiWindowFlags_Popup) - (b->Flags & ImGuiWindowFlags_Popup))
		return d;
	if (int d = (a->Flags & ImGuiWindowFlags_Tooltip) - (b->Flags & ImGuiWindowFlags_Tooltip))
		return d;
	if (int d = (a->Flags & ImGuiWindowFlags_ComboBox) - (b->Flags & ImGuiWindowFlags_ComboBox))
		return d;
	return 0;
}

static void AddWindowToSortedBuffer(ImVector<ImGuiWindow*>& outSortedWindows, ImGuiWindow* window)
{
	outSortedWindows.push_back(window);
	if (window->Active)
	{
		int count = window->DC.ChildWindows.Size;
		if (count > 1)
			qsort(window->DC.ChildWindows.begin(), (size_t)count, sizeof(ImGuiWindow*), ChildWindowComparer);
		for (int i = 0; i < count; i++)
		{
			ImGuiWindow* child = window->DC.ChildWindows[i];
			if (child->Active)
				AddWindowToSortedBuffer(outSortedWindows, child);
		}
	}
}

static void AddDrawListToRenderList(ImVector<ImDrawList*>& outRenderList, ImDrawList* drawList)
{
	if (!drawList->CmdBuffer.empty() && !drawList->VtxBuffer.empty())
	{
		if (drawList->CmdBuffer.back().ElemCount == 0)
			drawList->CmdBuffer.pop_back();
		outRenderList.push_back(drawList);

		//Check that drawList doesn't use more vertices than indexable (default ImDrawIdx = 2 bytes = 64K vertices)
		//If this assert triggers because you are drawing lots of stuff manually, A) workaround by calling BeginChild()/EndChild() to put your draw commands in multiple draw lists, B) #define ImDrawIdx to a 'unsigned int' in imconfig.h and render accordingly.
		const unsigned long long int maxVtxIdx = (unsigned long long int)1L << (sizeof(ImDrawIdx) * 8);
		(void)maxVtxIdx;
		IM_ASSERT((unsigned long long int)drawList->_VtxCurrentIdx <= maxVtxIdx);

		GImGui->IO.MetricsRenderVertices += drawList->VtxBuffer.Size;
		GImGui->IO.MetricsRenderIndices += drawList->IdxBuffer.Size;
	}
}

static void AddWindowToRenderList(ImVector<ImDrawList*>& outRenderList, ImGuiWindow* window)
{
	AddDrawListToRenderList(outRenderList, window->DrawList);
	for (int i = 0; i < window->DC.ChildWindows.Size; i++)
	{
		ImGuiWindow* child = window->DC.ChildWindows[i];
		if (!child->Active) //clipped children may have been marked not active
			continue;
		if ((child->Flags & ImGuiWindowFlags_Popup) && child->HiddenFrames > 0)
			continue;
		AddWindowToRenderList(outRenderList, child);
	}
}

static void PushClipRect(const ImRect& clipRect, bool clipped)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	ImRect cr = clipRect;
	if (clipped)	//Clip our argument with the current clip rect
		cr.Clip(window->ClipRect);
	cr.Max.x = ImMax(cr.Min.x, cr.Max.x);
	cr.Max.y = ImMax(cr.Min.y, cr.Max.y);

	IM_ASSERT(cr.Min.x <= cr.Max.x && cr.Min.y <= cr.Max.y);
	window->ClipRect = cr;
	window->DrawList->PushClipRect(ImVec4(cr.Min.x, cr.Min.y, cr.Max.x, cr.Max.y));
}

static void PopClipRect()
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    window->DrawList->PopClipRect();
    window->ClipRect = window->DrawList->_ClipRectStack.back();
}

//This is normally called by Render(). You may want to call it directly if you want to avoid calling Render() but the gain will be very minimal.
void ImGui::EndFrame()
{
	ImGuiState& g = *GImGui;
	IM_ASSERT(g.Initialized);	//Forgot to call ImGui::NewFrame()
	IM_ASSERT(g.FrameCountEnded != g.FrameCount);	//ImGui::EndFrame() called multiple times, or forgot to call ImGui::NewFrame() again

	//Render tooltip
	if (g.Tooltip[0])
	{
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(g.Tooltip);
		ImGui::EndTooltip();
	}

	//Hide implicit "Debug" window if it hasn't been used
	IM_ASSERT(g.CurrentWindowStack.Size == 1);	//Mismatched Begin/End
	if (g.CurrentWindow && !g.CurrentWindow->Accessed)
		g.CurrentWindow->Active = false;
	ImGui::End();

	//Click to focus window and start moving (after we're done with all our widgets)
	if (!g.ActiveId)
		g.MovedWindow = NULL;
	if (g.ActiveId == 0 && g.HoveredId == 0 && g.IO.MouseClicked[0])
	{
		if (!(g.FocusedWindow && !g.FocusedWindow->WasActive && g.FocusedWindow->Active))	//Unless we just made a popup appear
		{
			if (g.HoveredRootWindow != NULL)
			{
				FocusWindow(g.HoveredWindow);
				if (!(g.HoveredWindow->Flags & ImGuiWindowFlags_NoMove))
				{
					g.MovedWindow = g.HoveredWindow;
					SetActiveID(g.HoveredRootWindow->MoveID, g.HoveredRootWindow);
				}
			}
			else if (g.FocusedWindow != NULL && GetFrontMostModalRootWindow() == NULL)
			{
				//Clicking on void disable focus
				FocusWindow(NULL);
			}
		}
	}

	//Sort the window list so that all child windows are after their parent
	//We cannot do that on FocusWindow() because childs may not exist yet
	g.WindowsSortBuffer.resize(0);
	g.WindowsSortBuffer.reserve(g.Windows.Size);
	for (int i = 0; i != g.Windows.Size; i++)
	{
		ImGuiWindow* window = g.Windows[i];
		if (window->Flags & ImGuiWindowFlags_ChildWindow)	//if a child is active its parent will add it
		{
			if (window->Active)
				continue;
		}
		AddWindowToSortedBuffer(g.WindowsSortBuffer, window);
	}
	IM_ASSERT(g.Windows.Size == g.WindowsSortBuffer.Size);	//we done something wrong
	g.Windows.swap(g.WindowsSortBuffer);

	//Clear Input data for next frame
	g.IO.MouseWheel = 0.f;
	memset(g.IO.InputCharacters, 0, sizeof(g.IO.InputCharacters));

	g.FrameCountEnded = g.FrameCount;
}

void ImGui::Render()
{
	ImGuiState& g = *GImGui;
	IM_ASSERT(g.Initialized);	//Forgot to call ImGui::NewFrame()

	if (g.FrameCountEnded != g.FrameCount)
		ImGui::EndFrame();
	g.FrameCountRendered = g.FrameCount;

	//Skip render altogether if alpha is 0.0
	//Note that vertex buffers have been created and are wasted, so it is best practice that you don't create windows in the first place, or consistently respond to Begin() returning false.
	if (g.Style.Alpha > 0.f)
	{
		//Gather windows to render
		g.IO.MetricsRenderVertices = g.IO.MetricsRenderIndices = g.IO.MetricsActiveWindows = 0;
		for (int i = 0; i < IM_ARRAYSIZE(g.RenderDrawLists); i++)
			g.RenderDrawLists[i].resize(0);
		for (int i = 0; i != g.Windows.Size; i++)
		{
			ImGuiWindow* window = g.Windows[i];
			if (window->Active && window->HiddenFrames <= 0 && (window->Flags & (ImGuiWindowFlags_ChildWindow)) == 0)
			{
				//FIXME: Generalize this with a proper layering system so e.g. user can draw in specific layers, below text, ..
				g.IO.MetricsActiveWindows++;
				if (window->Flags & ImGuiWindowFlags_Popup)
					AddWindowToRenderList(g.RenderDrawLists[1], window);
				else if (window->Flags & ImGuiWindowFlags_Tooltip)
					AddWindowToRenderList(g.RenderDrawLists[2], window);
				else
					AddWindowToRenderList(g.RenderDrawLists[0], window);
			}
		}

		//Flatten layers
		int n = g.RenderDrawLists[0].Size;
		int flattenedSize = n;
		for (int i = 1; i < IM_ARRAYSIZE(g.RenderDrawLists); i++)
			flattenedSize += g.RenderDrawLists[i].Size;
		g.RenderDrawLists[0].resize(flattenedSize);
		for (int i = 1; i < IM_ARRAYSIZE(g.RenderDrawLists); i++)
		{
			ImVector<ImDrawList*>& layer = g.RenderDrawLists[i];
			if (layer.empty())
				continue;
			memcpy(&g.RenderDrawLists[0][n], &layer[0], layer.Size * sizeof(ImDrawList*));
			n += layer.Size;
		}

		//Draw software mouse cursor if requested
		if (g.IO.MouseDrawCursor)
		{
			const ImGuiMouseCursorData& cursorData = g.MouseCursorData[g.MouseCursor];
			const ImVec2 pos = g.IO.MousePos - cursorData.HotOffset;
			const ImVec2 size = cursorData.Size;
			const ImTextureID texID = g.IO.Fonts->TexID;
			g.OverlayDrawList.PushTextureID(texID);
			g.OverlayDrawList.AddImage(texID, pos + ImVec2(1, 0), pos + ImVec2(1, 0) + size, cursorData.TexUvMin[1], cursorData.TexUvMax[1], 0x30000000);	 //Shadow
			g.OverlayDrawList.AddImage(texID, pos + ImVec2(2, 0), pos + ImVec2(2, 0) + size, cursorData.TexUvMin[1], cursorData.TexUvMax[1], 0x30000000);	 //Shadow
			g.OverlayDrawList.AddImage(texID, pos, pos + size, cursorData.TexUvMin[1], cursorData.TexUvMax[1], 0xFF000000);	 //Black border
			g.OverlayDrawList.AddImage(texID, pos, pos + size, cursorData.TexUvMin[0], cursorData.TexUvMax[0], 0xFFFFFFFF);	 //White fill
			g.OverlayDrawList.PopTextureID();
		}
		if (!g.OverlayDrawList.VtxBuffer.empty())
			AddDrawListToRenderList(g.RenderDrawLists[0], &g.OverlayDrawList);

		//Setup draw data
		g.RenderDrawData.Valid = true;
		g.RenderDrawData.CmdLists = (g.RenderDrawLists[0].Size > 0) ? &g.RenderDrawLists[0][0] : NULL;
		g.RenderDrawData.CmdListsCount = g.RenderDrawLists[0].Size;
		g.RenderDrawData.TotalVtxCount = g.IO.MetricsRenderVertices;
		g.RenderDrawData.TotalIdxCount = g.IO.MetricsRenderIndices;

		//Render. If user hasn't set a callback then they may retrieve the draw data via GetDrawData()
		if (g.RenderDrawData.CmdListsCount > 0 && g.IO.RenderDrawListsFn != NULL)
			g.IO.RenderDrawListsFn(&g.RenderDrawData);
	}
}

//Find the optional ## from which we stop displaying text.
static const char*  FindTextDisplayEnd(const char* text, const char* textEnd)
{
	const char* textDisplayEnd = text;
	if (!textEnd)
		textEnd = (const char*)-1;

	ImGuiState& g = *GImGui;
	if (g.DisableHideTextAfterDoubleHash > 0)
	{
		while (textDisplayEnd < textEnd && *textDisplayEnd != '\0')
			textDisplayEnd++;
	}
	else
	{
		while (textDisplayEnd < textEnd && *textDisplayEnd != '\0' && (textDisplayEnd[0] != '#' || textDisplayEnd[1] != '#'))
			textDisplayEnd++;
	}
	return textDisplayEnd;
}

//Pass text data straight to log (without being displayed)
void ImGui::LogText(const char* fmt, ...)
{
	ImGuiState& g = *GImGui;
	if (!g.LogEnabled)
		return;

	va_list args;
	va_start(args, fmt);
	if (g.LogFile)
		vfprintf(g.LogFile, fmt, args);
	else
		g.LogClipboard->appendv(fmt, args);
	va_end(args);
}

//Internal version that takes a position to decide on newline placement and pad items according to their depth.
//We split text into individual lines to add current tree level padding
static void LogRenderedText(const ImVec2& refPos, const char* text, const char* textEnd)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = ImGui::GetCurrentWindowRead();

	if (!textEnd)
		textEnd = FindTextDisplayEnd(text, textEnd);

	const bool logNewLine = refPos.y > window->DC.LogLinePosY + 1;
	window->DC.LogLinePosY = refPos.y;

	const char* textRemaining = text;
	if (g.LogStartDepth > window->DC.TreeDepth)	//Re-adjust padding if we have popped out of our starting depth
		g.LogStartDepth = window->DC.TreeDepth;
	const int treeDepth = (window->DC.TreeDepth - g.LogStartDepth);
	for (;;)
	{
		//Split the string. Each new line (after a '\n') is followed by spacing corresponding to the current depth of our log entry.
		const char* lineEnd = textRemaining;
		while (lineEnd < textEnd)
		{
			if (*lineEnd == '\n')
				break;
			else
				lineEnd++;
		}
		if (lineEnd >= textEnd)
			lineEnd = NULL;

		const bool isFirstLine = (text == textRemaining);
		bool isLastLine = false;
		if (lineEnd == NULL)
		{
			isLastLine = true;
			lineEnd = textEnd;
		}
		if (lineEnd != NULL && !(isLastLine && (lineEnd - textRemaining) == 0))
		{
			const int charCount = (int)(lineEnd - textRemaining);
			if (logNewLine || !isFirstLine)
				ImGui::LogText(IM_NEWLINE "%*s%.*s", treeDepth * 4, "", charCount, textRemaining);
			else
				ImGui::LogText(" %.*s", charCount, textRemaining);
		}

		if (isLastLine)
			break;
		textRemaining = lineEnd + 1;
	}
}

//Internal ImGui functions to render text
//RenderText***() functions calls ImDrawList::AddText() calls ImBitmapFont::RenderText()
void ImGui::RenderText(ImVec2 pos, const char* text, const char* textEnd, bool hideTextAfterHash)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();

	//Hide anything after a '##' string
	const char* textDisplayEnd;
	if (hideTextAfterHash)
		textDisplayEnd = FindTextDisplayEnd(text, textEnd);
	else
	{
		if (!textEnd)
			textEnd = text + strlen(text);	//FIXME-OPT
		textDisplayEnd = textEnd;
	}

	const int textLen = (int)(textDisplayEnd - text);
	if (textLen > 0)
	{
		//Render
		window->DrawList->AddText(g.Font, g.FontSize, pos, window->Color(ImGuiCol_Text), text, textDisplayEnd);

		//Log as text
		if (g.LogEnabled)
			LogRenderedText(pos, text, textDisplayEnd);
	}
}

void ImGui::RenderTextWrapped(ImVec2 pos, const char* text, const char* textEnd, float wrapWidth)
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    if (!textEnd)
        textEnd = text + strlen(text);	//FIXME-OPT

    const int textLen = (int)(textEnd - text);
    if (textLen > 0)
    {
        window->DrawList->AddText(g.Font, g.FontSize, pos, window->Color(ImGuiCol_Text), text, textEnd, wrapWidth);
        if (g.LogEnabled)
            LogRenderedText(pos, text, textEnd);
    }
}

void ImGui::RenderTextClipped(const ImVec2& posMin, const ImVec2& posMax, const char* text, const char* textEnd, const ImVec2* textSizeIfKnown, ImGuiAlign align, const ImVec2* clipMin, const ImVec2* clipMax)
{
	//Hide anything after a '##' string
	const char* textDisplayEnd = FindTextDisplayEnd(text, textEnd);
	const int textLen = (int)(textDisplayEnd - text);
	if (textLen == 0)
		return;

	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();

	//Perform CPU side clipping for single clipped element to avoid using scissor state
	ImVec2 pos = posMin;
	const ImVec2 textSize = textSizeIfKnown ? *textSizeIfKnown : ImGui::CalcTextSize(text, textDisplayEnd, false, 0.f);

	if (!clipMax)
		clipMax = &posMax;
	bool needClipping = (pos.x + textSize.x >= clipMax->x) || (pos.y + textSize.y >= clipMax->y);
	if (!clipMin)
		clipMin = &posMin;
	else
		needClipping |= (pos.x < clipMin->x) || (pos.y < clipMin->y);

	//Align
	if (align & ImGuiAlign_Center)
		pos.x = ImMax(pos.x, (pos.x + posMax.x - textSize.x) * 0.5f);
	else if (align & ImGuiAlign_Right)
		pos.x = ImMax(pos.x, posMax.x - textSize.x);
	if (align & ImGuiAlign_VCenter)
		pos.y = ImMax(pos.y, (pos.y + posMax.y - textSize.y) * 0.5f);

	//Render
	if (needClipping)
	{
		ImVec4 fineClipRect(clipMin->x, clipMin->y, clipMax->x, clipMax->y);
		window->DrawList->AddText(g.Font, g.FontSize, pos, window->Color(ImGuiCol_Text), text, textDisplayEnd, 0.f, &fineClipRect);
	}
	else
		window->DrawList->AddText(g.Font, g.FontSize, pos, window->Color(ImGuiCol_Text), text, textDisplayEnd, 0.f, NULL);
	if (g.LogEnabled)
		LogRenderedText(pos, text, textDisplayEnd);
}

//Render a rectangle shaped with optional rounding and borders
void ImGui::RenderFrame(ImVec2 pMin, ImVec2 pMax, ImU32 fillCol, bool border, float rounding)
{
	ImGuiWindow* window = GetCurrentWindow();

	window->DrawList->AddRectFilled(pMin, pMax, fillCol, rounding);
	if (border && (window->Flags & ImGuiWindowFlags_ShowBorders))
	{
		window->DrawList->AddRect(pMin + ImVec2(1, 1), pMax, window->Color(ImGuiCol_BorderShadow), rounding);
		window->DrawList->AddRect(pMin, pMax - ImVec2(1, 1), window->Color(ImGuiCol_Border), rounding);
	}
}

//Render a triangle to denote expanded/collapsed state
void ImGui::RenderCollapseTriangle(ImVec2 pMin, bool opened, float scale, bool shadow)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();

	const float h = g.FontSize * 1.f;
	const float r = h * 0.4f * scale;
	ImVec2 center = pMin + ImVec2(h * 0.5f, h * 0.5f * scale);

	ImVec2 a, b, c;
	if (opened)
	{
		center.y -= r * 0.25f;
		a = center + ImVec2(0, 1) * r;
		b = center + ImVec2(-0.866f, -0.5f) * r;
		c = center + ImVec2(0.866f, -0.5f) * r;
	}
	else
	{
		a = center + ImVec2(1, 0)*r;
		b = center + ImVec2(-0.5f, 0.866f) * r;
		c = center + ImVec2(-0.5f, -0.866f) * r;
	}

	if (shadow && (window->Flags & ImGuiWindowFlags_ShowBorders) != 0)
		window->DrawList->AddTriangleFilled(a + ImVec2(2, 2), b + ImVec2(2, 2), c + ImVec2(2, 2), window->Color(ImGuiCol_BorderShadow));
	window->DrawList->AddTriangleFilled(a, b, c, window->Color(ImGuiCol_Text));
}

void ImGui::RenderCheckMark(ImVec2 pos, ImU32 col)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();

	ImVec2 a;
	ImVec2 b;
	ImVec2 c;
	float startX = (float)(int)(g.FontSize * 0.307f + 0.5f);
	float remThird = (float)(int)((g.FontSize - startX) / 3.f);
	a.x = pos.x + 0.5f + startX;
	b.x = a.x + remThird;
	c.x = a.x + remThird * 3.f;
	b.y = pos.y - 1.f + (float)(int)(g.Font->Ascent * (g.FontSize / g.Font->FontSize) + 0.5f) + (float)(int)(g.Font->DisplayOffset.y);
	a.y = b.y - remThird;
	c.y = b.y - remThird * 2.f;

	window->DrawList->PathLineTo(a);
	window->DrawList->PathLineTo(b);
	window->DrawList->PathLineTo(c);
	window->DrawList->PathStroke(col, false);
}

//Calculate text size. Text can be multi-line. Optionally ignore text after a ## marker.
//CalcTextSize("") should return ImVec2(0.f, GImGui->FontSize)
ImVec2 ImGui::CalcTextSize(const char* text, const char* textEnd, bool hideTextAfterDoubleHash, float wrapWidth)
{
	ImGuiState& g = *GImGui;

	const char* textDisplayEnd;
	if (hideTextAfterDoubleHash)
		textDisplayEnd = FindTextDisplayEnd(text, textEnd);	//Hide anything after a '##' string
	else
		textDisplayEnd = textEnd;

	ImFont* font = g.Font;
	const float fontSize = g.FontSize;
	ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, wrapWidth, text, textDisplayEnd, NULL);

	//Cancel out character spacing for the last character of a line (it is baked into glyph->XAdvance field)
	const float fontScale = fontSize / font->FontSize;
	const float characterSpacingX = 1.f * fontScale;
	if (textSize.x > 0.f)
		textSize.x -= characterSpacingX;

	return textSize;
}

//Helper to calculate coarse clipping of large list of evenly sized items.
//NB: Prefer using the ImGuiListClipper higher-level helper if you can!
//NB: 'itemsCount' is only used to clamp the result, if you don't know your count you can use INT_MAX
//If you are displaying thousands of items and you have a random access to the list, you can perform clipping yourself to save on CPU.
//{
//   float item_height = ImGui::GetTextLineHeightWithSpacing();
//   int display_start, display_end;
//   ImGui::CalcListClipping(count, item_height, &display_start, &display_end);	//calculate how many to clip/display
//   ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (display_start) * item_height);	//advance cursor
//   for (int i = display_start; i < display_end; i++)	//display only visible items
//       //TODO: display visible item
//   ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (count - display_end) * item_height);	//advance cursor
//}
void ImGui::CalcListClipping(int itemsCount, float itemsHeight, int* outItemsDisplayStart, int* outItemsDisplayEnd)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindowRead();
	if (g.LogEnabled)
	{
		//If logging is active, do not perform any clipping
		*outItemsDisplayStart = 0;
		*outItemsDisplayEnd = itemsCount;
		return;
	}

	const ImVec2 pos = window->DC.CursorPos;
	int start = (int)((window->ClipRect.Min.y - pos.y) / itemsHeight);
	int end = (int)((window->ClipRect.Max.y - pos.y) / itemsHeight);
	start = ImClamp(start, 0, itemsCount);
	end = ImClamp(end + 1, start, itemsCount);
	*outItemsDisplayStart = start;
	*outItemsDisplayEnd = end;
}

//Find window given position, search front-to-back
static ImGuiWindow* FindHoveredWindow(ImVec2 pos, bool excludingChilds)
{
	ImGuiState& g = *GImGui;
	for (int i = g.Windows.Size - 1; i >= 0; i--)
	{
		ImGuiWindow* window = g.Windows[i];
		if (!window->Active)
			continue;
		if (window->Flags & ImGuiWindowFlags_NoInputs)
			continue;
		if (excludingChilds && (window->Flags & ImGuiWindowFlags_ChildWindow) != 0)
			continue;

		//Using the clipped AABB so a child window will typically be clipped by its parent.
		ImRect bb(window->ClippedWindowRect.Min - g.Style.TouchExtraPadding, window->ClippedWindowRect.Max + g.Style.TouchExtraPadding);
		if (bb.Contains(pos))
			return window;
	}
	return NULL;
}

//Test if mouse cursor is hovering given rectangle
//NB- Rectangle is clipped by our current clip setting
//NB- Expand the rectangle to be generous on imprecise inputs systems (g.Style.TouchExtraPadding)
bool ImGui::IsMouseHoveringRect(const ImVec2& posMin, const ImVec2& posMax, bool clip)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindowRead();

	//Clip
	ImRect rectClipped(posMin, posMax);
	if (clip)
		rectClipped.Clip(window->ClipRect);

	//Expand for touch input
	const ImRect rectForTouch(rectClipped.Min - g.Style.TouchExtraPadding, rectClipped.Max + g.Style.TouchExtraPadding);
	return rectForTouch.Contains(g.IO.MousePos);
}

bool ImGui::IsMouseHoveringWindow()
{
	ImGuiState& g = *GImGui;
	return g.HoveredWindow == g.CurrentWindow;
}

bool ImGui::IsMouseHoveringAnyWindow()
{
	ImGuiState& g = *GImGui;
	return g.HoveredWindow != NULL;
}

bool ImGui::IsPosHoveringAnyWindow(const ImVec2& pos)
{
	return FindHoveredWindow(pos, false) != NULL;
}

static bool IsKeyPressedMap(ImGuiKey key, bool repeat)
{
	const int keyIndex = GImGui->IO.KeyMap[key];
	return ImGui::IsKeyPressed(keyIndex, repeat);
}

int ImGui::GetKeyIndex(ImGuiKey key)
{
	IM_ASSERT(key >= 0 && key < ImGuiKey_COUNT);
	return GImGui->IO.KeyMap[key];
}

bool ImGui::IsKeyDown(int keyIndex)
{
	if (keyIndex < 0)
		return false;
	IM_ASSERT(keyIndex >= 0 && keyIndex < IM_ARRAYSIZE(GImGui->IO.KeysDown));
	return GImGui->IO.KeysDown[keyIndex];
}

bool ImGui::IsKeyPressed(int keyIndex, bool repeat)
{
	ImGuiState& g = *GImGui;
	if (keyIndex < 0)
		return false;
	IM_ASSERT(keyIndex >= 0 && keyIndex < IM_ARRAYSIZE(g.IO.KeysDown));
	const float t = g.IO.KeysDownDuration[keyIndex];
	if (t == 0.f)
		return true;

	if (repeat && t > g.IO.KeyRepeatDelay)
	{
		float delay = g.IO.KeyRepeatDelay, rate = g.IO.KeyRepeatRate;
		if ((fmodf(t - delay, rate) > rate * 0.5f) != (fmodf(t - delay - g.IO.DeltaTime, rate) > rate * 0.5f))
			return true;
	}
	return false;
}

bool ImGui::IsKeyReleased(int keyIndex)
{
	ImGuiState& g = *GImGui;
	if (keyIndex < 0)
		return false;
	IM_ASSERT(keyIndex >= 0 && keyIndex < IM_ARRAYSIZE(g.IO.KeysDown));
	if (g.IO.KeysDownDurationPrev[keyIndex] >= 0.f && !g.IO.KeysDown[keyIndex])
		return true;
	return false;
}

bool ImGui::IsMouseDown(int button)
{
	ImGuiState& g = *GImGui;
	IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
	return g.IO.MouseDown[button];
}

bool ImGui::IsMouseClicked(int button, bool repeat)
{
	ImGuiState& g = *GImGui;
	IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
	const float t = g.IO.MouseDownDuration[button];
	if (t == 0.f)
		return true;

	if (repeat && t > g.IO.KeyRepeatDelay)
	{
		float delay = g.IO.KeyRepeatDelay, rate = g.IO.KeyRepeatRate;
		if ((fmodf(t - delay, rate) > rate * 0.5f) != (fmodf(t - delay - g.IO.DeltaTime, rate) > rate * 0.5f))
			return true;
	}

	return false;
}

bool ImGui::IsMouseReleased(int button)
{
	ImGuiState& g = *GImGui;
	IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
	return g.IO.MouseReleased[button];
}

bool ImGui::IsMouseDoubleClicked(int button)
{
	ImGuiState& g = *GImGui;
	IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
	return g.IO.MouseDoubleClicked[button];
}

bool ImGui::IsMouseDragging(int button, float lockThreshold)
{
	ImGuiState& g = *GImGui;
	IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
	if (!g.IO.MouseDown[button])
		return false;
	if (lockThreshold < 0.f)
		lockThreshold = g.IO.MouseDragThreshold;
	return g.IO.MouseDragMaxDistanceSqr[button] >= lockThreshold * lockThreshold;
}

ImVec2 ImGui::GetMousePos()
{
	return GImGui->IO.MousePos;
}

//NB: prefer to call right after BeginPopup(). At the time Selectable/MenuItem is activated, the popup is already closed!
ImVec2 ImGui::GetMousePosOnOpeningCurrentPopup()
{
	ImGuiState& g = *GImGui;
	if (g.CurrentPopupStack.Size > 0)
		return g.OpenedPopupStack[g.CurrentPopupStack.Size - 1].MousePosOnOpen;
	return g.IO.MousePos;
}

ImVec2 ImGui::GetMouseDragDelta(int button, float lockThreshold)
{
	ImGuiState& g = *GImGui;
	IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
	if (lockThreshold < 0.f)
		lockThreshold = g.IO.MouseDragThreshold;
	if (g.IO.MouseDown[button])
	{
		if (g.IO.MouseDragMaxDistanceSqr[button] >= lockThreshold * lockThreshold)
			return g.IO.MousePos - g.IO.MouseClickedPos[button];	//Assume we can only get active with left-mouse button (at the moment).
	}
	return ImVec2(0.f, 0.f);
}

void ImGui::ResetMouseDragDelta(int button)
{
	ImGuiState& g = *GImGui;
	IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
	//NB: We don't need to reset g.IO.MouseDragMaxDistanceSqr
	g.IO.MouseClickedPos[button] = g.IO.MousePos;
}

ImGuiMouseCursor ImGui::GetMouseCursor()
{
	return GImGui->MouseCursor;
}

void ImGui::SetMouseCursor(ImGuiMouseCursor cursorType)
{
	GImGui->MouseCursor = cursorType;
}

void ImGui::CaptureKeyboardFromApp()
{
	GImGui->CaptureKeyboardNextFrame = true;
}

void ImGui::CaptureMouseFromApp()
{
	GImGui->CaptureMouseNextFrame = true;
}

bool ImGui::IsItemHovered()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return window->DC.LastItemHoveredAndUsable;
}

bool ImGui::IsItemHoveredRect()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return window->DC.LastItemHoveredRect;
}

bool ImGui::IsItemActive()
{
	ImGuiState& g = *GImGui;
	if (g.ActiveId)
	{
		ImGuiWindow* window = GetCurrentWindowRead();
		return g.ActiveId == window->DC.LastItemID;
	}
	return false;
}

bool ImGui::IsAnyItemHovered()
{
	return GImGui->HoveredId != 0 || GImGui->HoveredIdPreviousFrame != 0;
}

bool ImGui::IsAnyItemActive()
{
	return GImGui->ActiveId != 0;
}

bool ImGui::IsItemVisible()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	ImRect r(window->ClipRect);
	return r.Overlaps(window->DC.LastItemRect);
}

ImVec2 ImGui::GetItemRectMin()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return window->DC.LastItemRect.Min;
}

ImVec2 ImGui::GetItemRectMax()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return window->DC.LastItemRect.Max;
}

ImVec2 ImGui::GetItemRectSize()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return window->DC.LastItemRect.GetSize();
}

ImVec2 ImGui::CalcItemRectClosestPoint(const ImVec2& pos, bool onEdge, float outward)
{
	ImGuiWindow* window = GetCurrentWindowRead();
	ImRect rect = window->DC.LastItemRect;
	rect.Expand(outward);
	return rect.GetClosestPoint(pos, onEdge);
}

//Tooltip is stored and turned into a BeginTooltip()/EndTooltip() sequence at the end of the frame. Each call override previous value.
void ImGui::SetTooltipV(const char* fmt, va_list args)
{
	ImGuiState& g = *GImGui;
	ImFormatStringV(g.Tooltip, IM_ARRAYSIZE(g.Tooltip), fmt, args);
}

void ImGui::SetTooltip(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	SetTooltipV(fmt, args);
	va_end(args);
}

static ImRect GetVisibleRect()
{
	ImGuiState& g = *GImGui;
	if (g.IO.DisplayVisibleMin.x != g.IO.DisplayVisibleMax.x && g.IO.DisplayVisibleMin.y != g.IO.DisplayVisibleMax.y)
		return ImRect(g.IO.DisplayVisibleMin, g.IO.DisplayVisibleMax);
	return ImRect(0.f, 0.f, g.IO.DisplaySize.x, g.IO.DisplaySize.y);
}

void ImGui::BeginTooltip()
{
	ImGuiState& g = *GImGui;
	ImGuiWindowFlags flags = ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize;
	ImGui::Begin("##Tooltip", NULL, ImVec2(0, 0), g.Style.Colors[ImGuiCol_TooltipBg].w, flags);
}

void ImGui::EndTooltip()
{
	IM_ASSERT(GetCurrentWindowRead()->Flags & ImGuiWindowFlags_Tooltip);
	ImGui::End();
}

static bool IsPopupOpen(ImGuiID id)
{
	ImGuiState& g = *GImGui;
	const bool opened = g.OpenedPopupStack.Size > g.CurrentPopupStack.Size && g.OpenedPopupStack[g.CurrentPopupStack.Size].PopupID == id;
	return opened;
}

//Mark popup as open. Popups are closed when user click outside, or activate a pressable item, or CloseCurrentPopup() is called within a BeginPopup()/EndPopup() block.
//Popup identifiers are relative to the current ID-stack (so OpenPopup and BeginPopup needs to be at the same level).
//One open popup per level of the popup hierarchy (NB: when assigning we reset the Window member of ImGuiPopupRef to NULL)
void ImGui::OpenPopup(const char* strID)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImGuiID id = window->GetID(strID);
	int currentStackSize = g.CurrentPopupStack.Size;
	ImGuiPopupRef popupRef = ImGuiPopupRef(id, window, window->GetID("##menus"), g.IO.MousePos);	//Tagged as new ref because constructor sets Window to NULL (we are passing the ParentWindow info here)
	if (g.OpenedPopupStack.Size < currentStackSize + 1)
		g.OpenedPopupStack.push_back(popupRef);
	else if (g.OpenedPopupStack[currentStackSize].PopupID != id)
	{
		g.OpenedPopupStack.resize(currentStackSize + 1);
		g.OpenedPopupStack[currentStackSize] = popupRef;
	}
}

static void CloseInactivePopups()
{
	ImGuiState& g = *GImGui;
	if (g.OpenedPopupStack.empty())
		return;

	//When popups are stacked, clicking on a lower level popups puts focus back to it and close popups above it.
	//Don't close our own child popup windows
	int n = 0;
	if (g.FocusedWindow)
	{
		for (n = 0; n < g.OpenedPopupStack.Size; n++)
		{
			ImGuiPopupRef& popup = g.OpenedPopupStack[n];
			if (!popup.Window)
				continue;
			IM_ASSERT((popup.Window->Flags & ImGuiWindowFlags_Popup) != 0);
			if (popup.Window->Flags & ImGuiWindowFlags_ChildWindow)
				continue;

			bool hasFocus = false;
			for (int m = n; m < g.OpenedPopupStack.Size && !hasFocus; m++)
				hasFocus = (g.OpenedPopupStack[m].Window && g.OpenedPopupStack[m].Window->RootWindow == g.FocusedWindow->RootWindow);
			if (!hasFocus)
				break;
		}
	}
	if (n < g.OpenedPopupStack.Size)	//This test is not required but it allows to set a useful breakpoint on the line below
		g.OpenedPopupStack.resize(n);
}

static ImGuiWindow* GetFrontMostModalRootWindow()
{
    ImGuiState& g = *GImGui;
	if (!g.OpenedPopupStack.empty())
	{
		if (ImGuiWindow* frontMostPopup = g.OpenedPopupStack.back().Window)
		{
			if (frontMostPopup->Flags & ImGuiWindowFlags_Modal)
				return frontMostPopup;
		}
	}
    return NULL;
}

static void ClosePopupToLevel(int remaining)
{
	ImGuiState& g = *GImGui;
	if (remaining > 0)
		ImGui::FocusWindow(g.OpenedPopupStack[remaining - 1].Window);
	else
		ImGui::FocusWindow(g.OpenedPopupStack[0].ParentWindow);
	g.OpenedPopupStack.resize(remaining);
}

static void ClosePopup(ImGuiID id)
{
	if (!IsPopupOpen(id))
		return;
	ImGuiState& g = *GImGui;
	ClosePopupToLevel(g.OpenedPopupStack.Size - 1);
}

//Close the popup we have begin-ed into.
void ImGui::CloseCurrentPopup()
{
	ImGuiState& g = *GImGui;
	int popupIDX = g.CurrentPopupStack.Size - 1;
	if (popupIDX < 0 || popupIDX > g.OpenedPopupStack.Size || g.CurrentPopupStack[popupIDX].PopupID != g.OpenedPopupStack[popupIDX].PopupID)
		return;
	while (popupIDX > 0 && g.OpenedPopupStack[popupIDX].Window && (g.OpenedPopupStack[popupIDX].Window->Flags & ImGuiWindowFlags_ChildMenu))
		popupIDX--;
	ClosePopupToLevel(popupIDX);
}

static void ClearSetNextWindowData()
{
	ImGuiState& g = *GImGui;
	g.SetNextWindowPosCond = g.SetNextWindowSizeCond = g.SetNextWindowContentSizeCond = g.SetNextWindowCollapsedCond = g.SetNextWindowFocus = 0;
}

static bool BeginPopupEx(const char* strID, ImGuiWindowFlags extraFlags)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	const ImGuiID id = window->GetID(strID);
	if (!IsPopupOpen(id))
	{
		ClearSetNextWindowData();	//We behave like Begin() and need to consume those values
		return false;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGuiWindowFlags flags = extraFlags | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | 
								ImGuiWindowFlags_AlwaysAutoResize;

	char name[32];
	if (flags & ImGuiWindowFlags_ChildMenu)
		ImFormatString(name, 20, "##menu_%d", g.CurrentPopupStack.Size);	//Recycle windows based on depth
	else
		ImFormatString(name, 20, "##popup_%08x", id);	//Not recycling, so we can close/open during the same frame
	float alpha = 1.f;

	bool opened = ImGui::Begin(name, NULL, ImVec2(0.f, 0.f), alpha, flags);
	if (!(window->Flags & ImGuiWindowFlags_ShowBorders))
		g.CurrentWindow->Flags &= ~ImGuiWindowFlags_ShowBorders;
	if (!opened)	//opened can be 'false' when the popup is completely clipped (e.g. zero size display)
		ImGui::EndPopup();

	return opened;
}

bool ImGui::BeginPopup(const char* strID) { return BeginPopupEx(strID, ImGuiWindowFlags_ShowBorders); }

bool ImGui::BeginPopupModal(const char* name, bool* pOpened, ImGuiWindowFlags extraFlags)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	const ImGuiID id = window->GetID(name);
	if (!IsPopupOpen(id))
	{
		ClearSetNextWindowData();	//We behave like Begin() and need to consume those values
		return false;
	}

	ImGuiWindowFlags flags = extraFlags | ImGuiWindowFlags_Popup | ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
	bool opened = ImGui::Begin(name, pOpened, ImVec2(0.f, 0.f), -1.f, flags);
	if (!opened || (pOpened && !*pOpened))	//Opened can be 'false' when the popup is completely clipped (e.g. zero size display)
	{
		ImGui::EndPopup();
		if (opened)
			ClosePopup(id);
		return false;
	}

	return opened;
}

void ImGui::EndPopup()
{
	ImGuiWindow* window = GetCurrentWindow();
	IM_ASSERT(window->Flags & ImGuiWindowFlags_Popup);
	IM_ASSERT(GImGui->CurrentPopupStack.Size > 0);
	ImGui::End();
	if (!(window->Flags & ImGuiWindowFlags_Modal))
		ImGui::PopStyleVar();
}

bool ImGui::BeginPopupContextItem(const char* strID, int mouseButton)
{
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(mouseButton))
		ImGui::OpenPopup(strID);
	return ImGui::BeginPopup(strID);
}

bool ImGui::BeginPopupContextWindow(bool alsoOverItems, const char* strID, int mouseButton)
{
    if (!strID)
		strID = "window_context_menu";
	if (ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(mouseButton))
	{
		if (alsoOverItems || !ImGui::IsAnyItemHovered())
			ImGui::OpenPopup(strID);
	}
    return ImGui::BeginPopup(strID);
}

bool ImGui::BeginPopupContextVoid(const char* strID, int mouseButton)
{
	if (!strID)
		strID = "void_context_menu";
	if (!ImGui::IsMouseHoveringAnyWindow() && ImGui::IsMouseClicked(mouseButton))
		ImGui::OpenPopup(strID);
	return ImGui::BeginPopup(strID);
}

bool ImGui::BeginChild(const char* strID, const ImVec2& sizeArg, bool border, ImGuiWindowFlags extraFlags)
{
	ImGuiWindow* window = GetCurrentWindow();
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;

	const ImVec2 contentAvail = ImGui::GetContentRegionAvail();
	ImVec2 size = ImRound(sizeArg);
	if (size.x <= 0.f)
	{
		if (size.x == 0.f)
			flags |= ImGuiWindowFlags_ChildWindowAutoFitX;
		size.x = ImMax(contentAvail.x, 4.f) - fabsf(size.x);	//Arbitrary minimum zero-ish child size of 4.f (0.f causing too much issues)
	}
	if (size.y <= 0.f)
	{
		if (size.y == 0.f)
			flags |= ImGuiWindowFlags_ChildWindowAutoFitY;
		size.y = ImMax(contentAvail.y, 4.f) - fabsf(size.y);
	}
	if (border)
		flags |= ImGuiWindowFlags_ShowBorders;
	flags |= extraFlags;

	char title[256];
	ImFormatString(title, IM_ARRAYSIZE(title), "%s.%s", window->Name, strID);

	const float alpha = 1.f;
	bool ret = ImGui::Begin(title, NULL, size, alpha, flags);

	if (!(window->Flags & ImGuiWindowFlags_ShowBorders))
		GetCurrentWindow()->Flags &= ~ImGuiWindowFlags_ShowBorders;

	return ret;
}

bool ImGui::BeginChild(ImGuiID id, const ImVec2& size, bool border, ImGuiWindowFlags extraFlags)
{
	char strID[32];
	ImFormatString(strID, IM_ARRAYSIZE(strID), "child_%08x", id);
	bool ret = ImGui::BeginChild(strID, size, border, extraFlags);
	return ret;
}

void ImGui::EndChild()
{
	ImGuiWindow* window = GetCurrentWindow();

	IM_ASSERT(window->Flags & ImGuiWindowFlags_ChildWindow);
	if ((window->Flags & ImGuiWindowFlags_ComboBox) || window->BeginCount > 1)
		ImGui::End();
	else
	{
		//When using auto-filling child window, we don't provide full width/height to ItemSize so that it doesn't feed back into automatic size-fitting.
		ImGuiState& g = *GImGui;
		ImVec2 sz = ImGui::GetWindowSize();
		if (window->Flags & ImGuiWindowFlags_ChildWindowAutoFitX) //Arbitrary minimum zeroish child size of 4.f
			sz.x = ImMax(4.f, sz.x - g.Style.WindowPadding.x);
		if (window->Flags & ImGuiWindowFlags_ChildWindowAutoFitY)
			sz.y = ImMax(4.f, sz.y - g.Style.WindowPadding.y);

		ImGui::End();

		window = GetCurrentWindow();
		ImRect bb(window->DC.CursorPos, window->DC.CursorPos + sz);
		ItemSize(sz);
		ItemAdd(bb, NULL);
	}
}

//Helper to create a child window / scrolling region that looks like a normal widget frame.
bool ImGui::BeginChildFrame(ImGuiID id, const ImVec2& size, ImGuiWindowFlags extraFlags)
{
	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, style.Colors[ImGuiCol_FrameBg]);
	ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, style.FrameRounding);
	return ImGui::BeginChild(id, size, false, ImGuiWindowFlags_NoMove | extraFlags);
}

void ImGui::EndChildFrame()
{
	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}

//Save and compare stack sizes on Begin()/End() to detect usage errors
static void CheckStacksSize(ImGuiWindow* window, bool write)
{
    //NOT checking: DC.ItemWidth, DC.AllowKeyboardFocus, DC.ButtonRepeat, DC.TextWrapPos (per window) to allow user to conveniently push once and not pop (they are cleared on Begin)
    ImGuiState& g = *GImGui;
    int* pBackup = &window->DC.StackSizesBackup[0];
    {
		int current = window->IDStack.Size;
		if (write)
			*pBackup = current;
		else
			IM_ASSERT(*pBackup == current);
		pBackup++;
	}	//User forgot PopID()
    {
		int current = window->DC.GroupStack.Size;
		if (write)
			*pBackup = current;
		else
			IM_ASSERT(*pBackup == current);
		pBackup++;
	}	//User forgot EndGroup()
    {
		int current = g.CurrentPopupStack.Size;
		if (write)
			*pBackup = current;
		else
			IM_ASSERT(*pBackup == current);
		pBackup++;
	}	//User forgot EndPopup()/EndMenu()
    {
		int current = g.ColorModifiers.Size;
		if (write)
			*pBackup = current;
		else
			IM_ASSERT(*pBackup == current);
		pBackup++;
	}	//User forgot PopStyleColor()
    {
		int current = g.StyleModifiers.Size;
		if (write)
			*pBackup = current;
		else
			IM_ASSERT(*pBackup == current);
		pBackup++;
	}	//User forgot PopStyleVar()
    {
		int current = g.FontStack.Size;
		if (write)
			*pBackup = current;
		else
			IM_ASSERT(*pBackup == current);
		pBackup++;
	}	//User forgot PopFont()
    IM_ASSERT(pBackup == window->DC.StackSizesBackup + IM_ARRAYSIZE(window->DC.StackSizesBackup));
}

static ImVec2 FindBestPopupWindowPos(const ImVec2& basePos, const ImVec2& size, ImGuiWindowFlags flags, int* lastDir, const ImRect& rInner)
{
	const ImGuiStyle& style = GImGui->Style;

	//Clamp into visible area while not overlapping the cursor
	ImRect rOuter(GetVisibleRect());
	rOuter.Reduce(style.DisplaySafeAreaPadding);
	ImVec2 basePosClamped = ImClamp(basePos, rOuter.Min, rOuter.Max - size);

	for (int n = (*lastDir != -1) ? -1 : 0; n < 4; n++)   //Right, down, up, left. Favor last used direction.
	{
		const int dir = (n == -1) ? *lastDir : n;
		ImRect rect(dir == 0 ? rInner.Max.x : rOuter.Min.x, dir == 1 ? rInner.Max.y : rOuter.Min.y, dir == 3 ? rInner.Min.x : rOuter.Max.x, dir == 2 ? rInner.Min.y : rOuter.Max.y);
		if (rect.GetWidth() < size.x || rect.GetHeight() < size.y)
			continue;
		*lastDir = dir;
		return ImVec2(dir == 0 ? rInner.Max.x : dir == 3 ? rInner.Min.x - size.x : basePosClamped.x, dir == 1 ? rInner.Max.y : dir == 2 ? rInner.Min.y - size.y : basePosClamped.y);
	}

	//Fallback
	*lastDir = -1;
	if (flags & ImGuiWindowFlags_Tooltip) //For tooltip we prefer avoiding the cursor at all cost even if it means that part of the tooltip won't be visible.
		return basePos + ImVec2(2, 2);

	//Otherwise try to keep within display
	ImVec2 pos = basePos;
	pos.x = ImMax(ImMin(pos.x + size.x, rOuter.Max.x) - size.x, rOuter.Min.x);
	pos.y = ImMax(ImMin(pos.y + size.y, rOuter.Max.y) - size.y, rOuter.Min.y);
	return pos;
}

static ImGuiWindow* FindWindowByName(const char* name)
{
	//FIXME-OPT: Store sorted hashes -> pointers.
	ImGuiState& g = *GImGui;
	ImGuiID id = ImHash(name, 0);
	for (int i = 0; i < g.Windows.Size; i++)
	{
		if (g.Windows[i]->ID == id)
			return g.Windows[i];
	}
	return NULL;
}

static ImGuiWindow* CreateNewWindow(const char* name, ImVec2 size, ImGuiWindowFlags flags)
{
	ImGuiState& g = *GImGui;

	//Create window the first time
	ImGuiWindow* window = (ImGuiWindow*)ImGui::MemAlloc(sizeof(ImGuiWindow));
	new(window) ImGuiWindow(name);
	window->Flags = flags;

	if (flags & ImGuiWindowFlags_NoSavedSettings)	//User can disable loading and saving of settings. Tooltip and child windows also don't store settings.
		window->Size = window->SizeFull = size;
	else
	{
		//Retrieve settings from .ini file
		//Use SetWindowPos() or SetNextWindowPos() with the appropriate condition flag to change the initial position of a window.
		window->PosFloat = ImVec2(60, 60);
		window->Pos = ImVec2((float)(int)window->PosFloat.x, (float)(int)window->PosFloat.y);

		ImGuiIniData* settings = FindWindowSettings(name);
		if (!settings)
			settings = AddWindowSettings(name);
		else
		{
			window->SetWindowPosAllowFlags &= ~ImGuiSetCond_FirstUseEver;
			window->SetWindowSizeAllowFlags &= ~ImGuiSetCond_FirstUseEver;
			window->SetWindowCollapsedAllowFlags &= ~ImGuiSetCond_FirstUseEver;
		}

		if (settings->Pos.x != FLT_MAX)
		{
			window->PosFloat = settings->Pos;
			window->Pos = ImVec2((float)(int)window->PosFloat.x, (float)(int)window->PosFloat.y);
			window->Collapsed = settings->Collapsed;
		}

		if (ImLengthSqr(settings->Size) > 0.00001f && !(flags & ImGuiWindowFlags_NoResize))
			size = settings->Size;
		window->Size = window->SizeFull = size;
	}

	if ((flags & ImGuiWindowFlags_AlwaysAutoResize) != 0)
	{
		window->AutoFitFramesX = window->AutoFitFramesY = 2;
		window->AutoFitOnlyGrows = false;
	}
	else
	{
		if (window->Size.x <= 0.f)
			window->AutoFitFramesX = 2;
		if (window->Size.y <= 0.f)
			window->AutoFitFramesY = 2;
		window->AutoFitOnlyGrows = (window->AutoFitFramesX > 0) || (window->AutoFitFramesY > 0);
	}

	g.Windows.push_back(window);
	return window;
}

//Push a new ImGui window to add widgets to.
//- A default window called "Debug" is automatically stacked at the beginning of every frame so you can use widgets without explicitly calling a Begin/End pair.
//- Begin/End can be called multiple times during the frame with the same window name to append content.
//- 'sizeOnFirstUse' for a regular window denote the initial size for first-time creation (no saved data) and isn't that useful. Use SetNextWindowSize() prior to calling Begin() for more flexible window manipulation.
//- The window name is used as a unique identifier to preserve window information across frames (and save rudimentary information to the .ini file).
//  You can use the "##" or "###" markers to use the same label with different id, or same id with different label. See documentation at the top of this file.
//- Return false when window is collapsed, so you can early out in your code. You always need to call ImGui::End() even if false is returned.
//- Passing 'bool* pOpened' displays a Close button on the upper-right corner of the window, the pointed value will be set to false when the button is pressed.
//- Passing non-zero 'size' is roughly equivalent to calling SetNextWindowSize(size, ImGuiSetCond_FirstUseEver) prior to calling Begin().
bool ImGui::Begin(const char* name, bool* pOpened, ImGuiWindowFlags flags) { return ImGui::Begin(name, pOpened, ImVec2(0.f, 0.f), -1.f, flags); }

bool ImGui::Begin(const char* name, bool* pOpened, const ImVec2& sizeOnFirstUse, float bgAlpha, ImGuiWindowFlags flags)
{
	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	IM_ASSERT(name != NULL);	//Window name required
	IM_ASSERT(g.Initialized);	//Forgot to call ImGui::NewFrame()
	IM_ASSERT(g.FrameCountEnded != g.FrameCount);	//Called ImGui::Render() or ImGui::EndFrame() and haven't called ImGui::NewFrame() again yet

	if (flags & ImGuiWindowFlags_NoInputs)
		flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

	//Find or create
	bool windowIsNew = false;
	ImGuiWindow* window = FindWindowByName(name);
	if (!window)
	{
		window = CreateNewWindow(name, sizeOnFirstUse, flags);
		windowIsNew = true;
	}

	const int currentFrame = ImGui::GetFrameCount();
	const bool firstBeginOfTheFrame = (window->LastFrameActive != currentFrame);
	if (firstBeginOfTheFrame)
		window->Flags = (ImGuiWindowFlags)flags;
	else
		flags = window->Flags;

	//Add to stack
	ImGuiWindow* parentWindow = !g.CurrentWindowStack.empty() ? g.CurrentWindowStack.back() : NULL;
	g.CurrentWindowStack.push_back(window);
	SetCurrentWindow(window);
	CheckStacksSize(window, true);
	IM_ASSERT(parentWindow != NULL || !(flags & ImGuiWindowFlags_ChildWindow));

	bool windowWasActive = (window->LastFrameActive == currentFrame - 1);	//Not using !WasActive because the implicit "Debug" window would always toggle off->on
	if (flags & ImGuiWindowFlags_Popup)
	{
		ImGuiPopupRef& popupRef = g.OpenedPopupStack[g.CurrentPopupStack.Size];
		windowWasActive &= (window->PopupID == popupRef.PopupID);
		windowWasActive &= (window == popupRef.Window);
		popupRef.Window = window;
		g.CurrentPopupStack.push_back(popupRef);
		window->PopupID = popupRef.PopupID;
	}

	const bool windowAppearingAfterBeingHidden = (window->HiddenFrames == 1);

	//Process SetNextWindow***() calls
	bool windowPosSetByAPI = false;
	bool windowSizeSetByAPI = false;
	if (g.SetNextWindowPosCond)
	{
		const ImVec2 backupCursorPos = window->DC.CursorPos;	//FIXME: not sure of the exact reason of this anymore :( need to look into that.
		if (!windowWasActive || windowAppearingAfterBeingHidden)
			window->SetWindowPosAllowFlags |= ImGuiSetCond_Appearing;
		windowPosSetByAPI = (window->SetWindowPosAllowFlags & g.SetNextWindowPosCond) != 0;
		if (windowPosSetByAPI && ImLengthSqr(g.SetNextWindowPosVal - ImVec2(-FLT_MAX, -FLT_MAX)) < 0.001f)
		{
			window->SetWindowPosCenterWanted = true;	//May be processed on the next frame if this is our first frame and we are measuring size
			window->SetWindowPosAllowFlags &= ~(ImGuiSetCond_Once | ImGuiSetCond_FirstUseEver | ImGuiSetCond_Appearing);
		}
		else
			SetWindowPos(window, g.SetNextWindowPosVal, g.SetNextWindowPosCond);
		window->DC.CursorPos = backupCursorPos;
		g.SetNextWindowPosCond = 0;
	}
	if (g.SetNextWindowSizeCond)
	{
		if (!windowWasActive || windowAppearingAfterBeingHidden)
			window->SetWindowSizeAllowFlags |= ImGuiSetCond_Appearing;
		windowSizeSetByAPI = (window->SetWindowSizeAllowFlags & g.SetNextWindowSizeCond) != 0;
		SetWindowSize(window, g.SetNextWindowSizeVal, g.SetNextWindowSizeCond);
		g.SetNextWindowSizeCond = 0;
	}
	if (g.SetNextWindowContentSizeCond)
	{
		window->SizeContentsExplicit = g.SetNextWindowContentSizeVal;
		g.SetNextWindowContentSizeCond = 0;
	}
	else if (firstBeginOfTheFrame)
		window->SizeContentsExplicit = ImVec2(0.f, 0.f);
	if (g.SetNextWindowCollapsedCond)
	{
		if (!windowWasActive || windowAppearingAfterBeingHidden)
			window->SetWindowCollapsedAllowFlags |= ImGuiSetCond_Appearing;
		SetWindowCollapsed(window, g.SetNextWindowCollapsedVal, g.SetNextWindowCollapsedCond);
		g.SetNextWindowCollapsedCond = 0;
	}
	if (g.SetNextWindowFocus)
	{
		ImGui::SetWindowFocus();
		g.SetNextWindowFocus = false;
	}

	//Update known root window (if we are a child window, otherwise window == window->RootWindow)
	int rootIDX, rootNonPopupIDX;
	for (rootIDX = g.CurrentWindowStack.Size - 1; rootIDX > 0; rootIDX--)
	{
		if (!(g.CurrentWindowStack[rootIDX]->Flags & ImGuiWindowFlags_ChildWindow))
			break;
	}
	for (rootNonPopupIDX = rootIDX; rootNonPopupIDX > 0; rootNonPopupIDX--)
	{
		if (!(g.CurrentWindowStack[rootNonPopupIDX]->Flags & (ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_Popup)))
			break;
	}
	window->RootWindow = g.CurrentWindowStack[rootIDX];
	window->RootNonPopupWindow = g.CurrentWindowStack[rootNonPopupIDX];	//This is merely for displaying the TitleBgActive color.

	//Default alpha
	if (bgAlpha < 0.f)
		bgAlpha = style.WindowFillAlphaDefault;

	//When reusing window again multiple times a frame, just append content (don't need to setup again)
	if (firstBeginOfTheFrame)
	{
		window->Active = true;
		window->BeginCount = 0;
		window->DrawList->Clear();
		window->ClipRect = ImVec4(-FLT_MAX, -FLT_MAX, +FLT_MAX, +FLT_MAX);
		window->LastFrameActive = currentFrame;
		window->IDStack.resize(1);

		//Setup texture, outer clipping rectangle
		window->DrawList->PushTextureID(g.Font->ContainerAtlas->TexID);
		if ((flags & ImGuiWindowFlags_ChildWindow) && !(flags & (ImGuiWindowFlags_ComboBox | ImGuiWindowFlags_Popup)))
			PushClipRect(parentWindow->ClipRect);
		else
			PushClipRect(GetVisibleRect());

		//New windows appears in front
		if (!windowWasActive)
		{
			window->AutoPosLastDirection = -1;

			if (!(flags & ImGuiWindowFlags_NoFocusOnAppearing))
			{
				if (!(flags & (ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_Tooltip)) || (flags & ImGuiWindowFlags_Popup))
					FocusWindow(window);
			}

			//Popup first latch mouse position, will position itself when it appears next frame
			if ((flags & ImGuiWindowFlags_Popup) != 0 && !windowPosSetByAPI)
				window->PosFloat = g.IO.MousePos;
		}

		//Collapse window by double-clicking on title bar
		//At this point we don't have a clipping rectangle setup yet, so we can use the title bar area for hit detection and drawing
		if (!(flags & ImGuiWindowFlags_NoTitleBar) && !(flags & ImGuiWindowFlags_NoCollapse))
		{
			ImRect titleBarRect = window->TitleBarRect();
			if (g.HoveredWindow == window && IsMouseHoveringRect(titleBarRect.Min, titleBarRect.Max) && g.IO.MouseDoubleClicked[0])
			{
				window->Collapsed = !window->Collapsed;
				if (!(flags & ImGuiWindowFlags_NoSavedSettings))
					MarkSettingsDirty();
				FocusWindow(window);
			}
		}
		else
			window->Collapsed = false;

		//SIZE

		//Save contents size from last frame for auto-fitting (unless explicitly specified)
		window->SizeContents.x = (window->SizeContentsExplicit.x != 0.f) ? window->SizeContentsExplicit.x : ((windowIsNew ? 0.f : window->DC.CursorMaxPos.x - window->Pos.x) + window->Scroll.x);
		window->SizeContents.y = (window->SizeContentsExplicit.y != 0.f) ? window->SizeContentsExplicit.y : ((windowIsNew ? 0.f : window->DC.CursorMaxPos.y - window->Pos.y) + window->Scroll.y);

		//Hide popup/tooltip window when first appearing while we measure size (because we recycle them)
		if (window->HiddenFrames > 0)
			window->HiddenFrames--;
		if ((flags & (ImGuiWindowFlags_Popup | ImGuiWindowFlags_Tooltip)) != 0 && !windowWasActive)
		{
			window->HiddenFrames = 1;
			if (flags & ImGuiWindowFlags_AlwaysAutoResize)
			{
				if (!windowSizeSetByAPI)
					window->Size = window->SizeFull = ImVec2(0.f, 0.f);
				window->SizeContents = ImVec2(0.f, 0.f);
			}
		}

		//Lock window padding so that altering the ShowBorders flag for children doesn't have side-effects.
		window->WindowPadding = ((flags & ImGuiWindowFlags_ChildWindow) && !(flags & 
																				(ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_ComboBox | ImGuiWindowFlags_Popup))) ? ImVec2(0, 0) : style.WindowPadding;

		//Calculate auto-fit size
		ImVec2 sizeAutoFit;
		if ((flags & ImGuiWindowFlags_Tooltip) != 0)	//Tooltip always resize. We keep the spacing symmetric on both axises for aesthetic purpose.
			sizeAutoFit = window->SizeContents + window->WindowPadding - ImVec2(0.f, style.ItemSpacing.y);
		else
		{
			sizeAutoFit = ImClamp(window->SizeContents + window->WindowPadding, style.WindowMinSize, ImMax(style.WindowMinSize, g.IO.DisplaySize - window->WindowPadding));

			//Handling case of auto fit window not fitting in screen on one axis, we are growing auto fit size on the other axis to compensate for expected scrollbar. FIXME: Might turn bigger than DisplaySize-WindowPadding.
			if (sizeAutoFit.x < window->SizeContents.x && !(flags & ImGuiWindowFlags_NoScrollbar) && (flags & ImGuiWindowFlags_HorizontalScrollbar))
				sizeAutoFit.y += style.ScrollbarSize;
			if (sizeAutoFit.y < window->SizeContents.y && !(flags & ImGuiWindowFlags_NoScrollbar))
				sizeAutoFit.x += style.ScrollbarSize;
			sizeAutoFit.y = ImMax(sizeAutoFit.y - style.ItemSpacing.y, 0.f);
		}

		//Handle automatic resize
		if (window->Collapsed)
		{
			//We still process initial auto-fit on collapsed windows to get a window width,
			//But otherwise we don't honor ImGuiWindowFlags_AlwaysAutoResize when collapsed.
			if (window->AutoFitFramesX > 0)
				window->SizeFull.x = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.x, sizeAutoFit.x) : sizeAutoFit.x;
			if (window->AutoFitFramesY > 0)
				window->SizeFull.y = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.y, sizeAutoFit.y) : sizeAutoFit.y;
			window->Size = window->TitleBarRect().GetSize();
		}
		else
		{
			if ((flags & ImGuiWindowFlags_AlwaysAutoResize) && !windowSizeSetByAPI)
				window->SizeFull = sizeAutoFit;
			else if ((window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0) && !windowSizeSetByAPI)
			{
				//Auto-fit only grows during the first few frames
				if (window->AutoFitFramesX > 0)
					window->SizeFull.x = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.x, sizeAutoFit.x) : sizeAutoFit.x;
				if (window->AutoFitFramesY > 0)
					window->SizeFull.y = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.y, sizeAutoFit.y) : sizeAutoFit.y;
				if (!(flags & ImGuiWindowFlags_NoSavedSettings))
					MarkSettingsDirty();
			}
			window->Size = window->SizeFull;
		}

		//Minimum window size
		if (!(flags & (ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_AlwaysAutoResize)))
		{
			window->SizeFull = ImMax(window->SizeFull, style.WindowMinSize);
			if (!window->Collapsed)
				window->Size = window->SizeFull;
		}

		//POSITION

		//Position child window
		if (flags & ImGuiWindowFlags_ChildWindow)
			parentWindow->DC.ChildWindows.push_back(window);
		if ((flags & ImGuiWindowFlags_ChildWindow) && !(flags & ImGuiWindowFlags_Popup))
		{
			window->Pos = window->PosFloat = parentWindow->DC.CursorPos;
			window->Size = window->SizeFull = sizeOnFirstUse;	//NB: argument name 'sizeOnFirstUse' misleading here, it's really just 'size' as provided by user.
		}

		bool windowPosCenter = false;
		windowPosCenter |= (window->SetWindowPosCenterWanted && window->HiddenFrames == 0);
		windowPosCenter |= ((flags & ImGuiWindowFlags_Modal) && !windowPosSetByAPI && windowAppearingAfterBeingHidden);
		if (windowPosCenter)
		{
			//Center (any sort of window)
			ImRect fullscreenRect(GetVisibleRect());
			SetWindowPos(ImMax(style.DisplaySafeAreaPadding, fullscreenRect.GetCenter() - window->SizeFull * 0.5f));
		}
		else if (flags & ImGuiWindowFlags_ChildMenu)
		{
			IM_ASSERT(windowPosSetByAPI);
			ImRect rectToAvoid;
			if (parentWindow->DC.MenuBarAppending)
				rectToAvoid = ImRect(-FLT_MAX, parentWindow->Pos.y + parentWindow->TitleBarHeight(), FLT_MAX, parentWindow->Pos.y + parentWindow->TitleBarHeight() + parentWindow->MenuBarHeight());
			else
				rectToAvoid = ImRect(parentWindow->Pos.x + style.ItemSpacing.x, -FLT_MAX, parentWindow->Pos.x + parentWindow->Size.x - style.ItemSpacing.x - parentWindow->ScrollbarSizes.x, FLT_MAX);	
										//We want some overlap to convey the relative depth of each popup (here hard-coded to 4)
			window->PosFloat = FindBestPopupWindowPos(window->PosFloat, window->Size, flags, &window->AutoPosLastDirection, rectToAvoid);
		}
		else if ((flags & ImGuiWindowFlags_Popup) != 0 && !windowPosSetByAPI && windowAppearingAfterBeingHidden)
		{
			ImRect rectToAvoid(window->PosFloat.x - 1, window->PosFloat.y - 1, window->PosFloat.x + 1, window->PosFloat.y + 1);
			window->PosFloat = FindBestPopupWindowPos(window->PosFloat, window->Size, flags, &window->AutoPosLastDirection, rectToAvoid);
		}

		//Position tooltip (always follows mouse)
		if ((flags & ImGuiWindowFlags_Tooltip) != 0 && !windowPosSetByAPI)
		{
			ImRect rectToAvoid(g.IO.MousePos.x - 16, g.IO.MousePos.y - 8, g.IO.MousePos.x + 24, g.IO.MousePos.y + 24); //FIXME: Completely hard-coded. Perhaps center on cursor hit-point instead?
			window->PosFloat = FindBestPopupWindowPos(g.IO.MousePos, window->Size, flags, &window->AutoPosLastDirection, rectToAvoid);
		}

		//User moving window (at the beginning of the frame to avoid input lag or sheering). Only valid for root windows.
		KeepAliveID(window->MoveID);
		if (g.ActiveId == window->MoveID)
		{
			if (g.IO.MouseDown[0])
			{
				if (!(flags & ImGuiWindowFlags_NoMove))
				{
					window->PosFloat += g.IO.MouseDelta;
					if (!(flags & ImGuiWindowFlags_NoSavedSettings))
						MarkSettingsDirty();
				}
				IM_ASSERT(g.MovedWindow != NULL);
				FocusWindow(g.MovedWindow);
			}
			else
			{
				SetActiveID(0);
				g.MovedWindow = NULL;   //Not strictly necessary but doing it for sanity.
			}
		}

		//Clamp position so it stays visible
		if (!(flags & ImGuiWindowFlags_ChildWindow) && !(flags & ImGuiWindowFlags_Tooltip))
		{
			if (!windowPosSetByAPI && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 && g.IO.DisplaySize.x > 0.f && g.IO.DisplaySize.y > 0.f)
				//Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
			{
				ImVec2 padding = ImMax(style.DisplayWindowPadding, style.DisplaySafeAreaPadding);
				window->PosFloat = ImMax(window->PosFloat + window->Size, padding) - window->Size;
				window->PosFloat = ImMin(window->PosFloat, g.IO.DisplaySize - padding);
			}
		}
		window->Pos = ImVec2((float)(int)window->PosFloat.x, (float)(int)window->PosFloat.y);

		//Default item width. Make it proportional to window size if window manually resizes
		if (window->Size.x > 0.f && !(flags & ImGuiWindowFlags_Tooltip) && !(flags & ImGuiWindowFlags_AlwaysAutoResize))
			window->ItemWidthDefault = (float)(int)(window->Size.x * 0.65f);
		else
			window->ItemWidthDefault = (float)(int)(g.FontSize * 16.f);

		//Prepare for focus requests
		window->FocusIdxAllRequestCurrent = (window->FocusIdxAllRequestNext == IM_INT_MAX || 
												window->FocusIdxAllCounter == -1) ? IM_INT_MAX : (window->FocusIdxAllRequestNext + (window->FocusIdxAllCounter + 1)) % (window->FocusIdxAllCounter + 1);
		window->FocusIdxTabRequestCurrent = (window->FocusIdxTabRequestNext == IM_INT_MAX || 
												window->FocusIdxTabCounter == -1) ? IM_INT_MAX : (window->FocusIdxTabRequestNext + (window->FocusIdxTabCounter + 1)) % (window->FocusIdxTabCounter + 1);
		window->FocusIdxAllCounter = window->FocusIdxTabCounter = -1;
		window->FocusIdxAllRequestNext = window->FocusIdxTabRequestNext = IM_INT_MAX;

		//Apply scrolling
		if (window->ScrollTarget.x < FLT_MAX)
		{
			window->Scroll.x = window->ScrollTarget.x;
			window->ScrollTarget.x = FLT_MAX;
		}
		if (window->ScrollTarget.y < FLT_MAX)
		{
			float centerRatio = window->ScrollTargetCenterRatio.y;
			window->Scroll.y = window->ScrollTarget.y - ((1.f - centerRatio) * window->TitleBarHeight()) - (centerRatio * window->SizeFull.y);
			window->ScrollTarget.y = FLT_MAX;
		}
		window->Scroll = ImMax(window->Scroll, ImVec2(0.f, 0.f));
		if (!window->Collapsed && !window->SkipItems)
			window->Scroll = ImMin(window->Scroll, ImMax(ImVec2(0.f, 0.f), window->SizeContents - window->SizeFull + window->ScrollbarSizes));

		//Modal window darkens what is behind them
		if ((flags & ImGuiWindowFlags_Modal) != 0 && window == GetFrontMostModalRootWindow())
		{
			ImRect fullscreenRect = GetVisibleRect();
			window->DrawList->AddRectFilled(fullscreenRect.Min, fullscreenRect.Max, window->Color(ImGuiCol_ModalWindowDarkening, g.ModalWindowDarkeningRatio));
		}

		//Draw window + handle manual resize
		ImRect titleBarRect = window->TitleBarRect();
		const float windowRounding = (flags & ImGuiWindowFlags_ChildWindow) ? style.ChildWindowRounding : style.WindowRounding;
		if (window->Collapsed)
		{
			//Draw title bar only
			window->DrawList->AddRectFilled(titleBarRect.GetTL(), titleBarRect.GetBR(), window->Color(ImGuiCol_TitleBgCollapsed), windowRounding);
			if (flags & ImGuiWindowFlags_ShowBorders)
			{
				window->DrawList->AddRect(titleBarRect.GetTL() + ImVec2(1, 1), titleBarRect.GetBR() + ImVec2(1, 1), window->Color(ImGuiCol_BorderShadow), windowRounding);
				window->DrawList->AddRect(titleBarRect.GetTL(), titleBarRect.GetBR(), window->Color(ImGuiCol_Border), windowRounding);
			}
		}
		else
		{
			ImU32 resizeCol = 0;
			const float resizeCornerSize = ImMax(g.FontSize * 1.35f, windowRounding + 1.f + g.FontSize * 0.2f);
			if (!(flags & ImGuiWindowFlags_AlwaysAutoResize) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 && !(flags & ImGuiWindowFlags_NoResize))
			{
				//Manual resize
				const ImVec2 br = window->Rect().GetBR();
				const ImRect resizeRect(br - ImVec2(resizeCornerSize * 0.75f, resizeCornerSize * 0.75f), br);
				const ImGuiID resizeID = window->GetID("#RESIZE");
				bool hovered;
				bool held;
				ButtonBehavior(resizeRect, resizeID, &hovered, &held, ImGuiButtonFlags_FlattenChilds);
				resizeCol = window->Color(held ? ImGuiCol_ResizeGripActive : hovered ? ImGuiCol_ResizeGripHovered : ImGuiCol_ResizeGrip);

				if (hovered || held)
					g.MouseCursor = ImGuiMouseCursor_ResizeNWSE;

				if (g.HoveredWindow == window && held && g.IO.MouseDoubleClicked[0])
				{
					//Manual auto-fit when double-clicking
					window->SizeFull = sizeAutoFit;
					if (!(flags & ImGuiWindowFlags_NoSavedSettings))
						MarkSettingsDirty();
					SetActiveID(0);
				}
				else if (held)
				{
					window->SizeFull = ImMax(window->SizeFull + g.IO.MouseDelta, style.WindowMinSize);
					if (!(flags & ImGuiWindowFlags_NoSavedSettings))
						MarkSettingsDirty();
				}

				window->Size = window->SizeFull;
				titleBarRect = window->TitleBarRect();
			}

			//Scrollbars
			window->ScrollbarY = (window->SizeContents.y > window->Size.y + style.ItemSpacing.y) && !(flags & ImGuiWindowFlags_NoScrollbar);
			window->ScrollbarX = (window->SizeContents.x > window->Size.x - (window->ScrollbarY ? style.ScrollbarSize : 0.f) - window->WindowPadding.x) && 
									!(flags & ImGuiWindowFlags_NoScrollbar) && (flags & ImGuiWindowFlags_HorizontalScrollbar);
			window->ScrollbarSizes = ImVec2(window->ScrollbarY ? style.ScrollbarSize : 0.f, window->ScrollbarX ? style.ScrollbarSize : 0.f);

			//Window background
			if (bgAlpha > 0.f)
			{
				if ((flags & ImGuiWindowFlags_ComboBox) != 0)
					window->DrawList->AddRectFilled(window->Pos, window->Pos + window->Size, window->Color(ImGuiCol_ComboBg, bgAlpha), windowRounding);
				else if ((flags & ImGuiWindowFlags_Tooltip) != 0)
					window->DrawList->AddRectFilled(window->Pos, window->Pos + window->Size, window->Color(ImGuiCol_TooltipBg, bgAlpha), windowRounding);
				else if ((flags & ImGuiWindowFlags_Popup) != 0)
					window->DrawList->AddRectFilled(window->Pos, window->Pos + window->Size, window->Color(ImGuiCol_WindowBg, bgAlpha), windowRounding);
				else if ((flags & ImGuiWindowFlags_ChildWindow) != 0)
					window->DrawList->AddRectFilled(window->Pos, window->Pos + window->Size, window->Color(ImGuiCol_ChildWindowBg, bgAlpha), windowRounding);
				else
					window->DrawList->AddRectFilled(window->Pos, window->Pos + window->Size, window->Color(ImGuiCol_WindowBg, bgAlpha), windowRounding);
			}

			//Title bar
			if (!(flags & ImGuiWindowFlags_NoTitleBar))
				window->DrawList->AddRectFilled(titleBarRect.GetTL(), titleBarRect.GetBR(), 
												window->Color((g.FocusedWindow && window->RootNonPopupWindow == g.FocusedWindow->RootNonPopupWindow) ? ImGuiCol_TitleBgActive : ImGuiCol_TitleBg), 
																windowRounding, 1 | 2);

			//Menu bar
			if (flags & ImGuiWindowFlags_MenuBar)
			{
				ImRect menuBarRect = window->MenuBarRect();
				window->DrawList->AddRectFilled(menuBarRect.GetTL(), menuBarRect.GetBR(), window->Color(ImGuiCol_MenuBarBg), (flags & ImGuiWindowFlags_NoTitleBar) ? windowRounding : 0.f, 1 | 2);
			}

			//Borders
			if (flags & ImGuiWindowFlags_ShowBorders)
			{
				window->DrawList->AddRect(window->Pos + ImVec2(1, 1), window->Pos + window->Size + ImVec2(1, 1), window->Color(ImGuiCol_BorderShadow), windowRounding);
				window->DrawList->AddRect(window->Pos, window->Pos + window->Size, window->Color(ImGuiCol_Border), windowRounding);
				if (!(flags & ImGuiWindowFlags_NoTitleBar))
					window->DrawList->AddLine(titleBarRect.GetBL(), titleBarRect.GetBR(), window->Color(ImGuiCol_Border));
			}

			//Scrollbars
			if (window->ScrollbarX)
				Scrollbar(window, true);
			if (window->ScrollbarY)
				Scrollbar(window, false);

			//Render resize grip
			//(after the input handling so we don't have a frame of latency)
			if (!(flags & ImGuiWindowFlags_NoResize))
			{
				const ImVec2 br = window->Rect().GetBR();
				window->DrawList->PathLineTo(br + ImVec2(-resizeCornerSize, 0.f));
				window->DrawList->PathLineTo(br + ImVec2(0.f, -resizeCornerSize));
				window->DrawList->PathArcToFast(ImVec2(br.x - windowRounding, br.y - windowRounding), windowRounding, 0, 3);
				window->DrawList->PathFill(resizeCol);
			}
		}

		//Setup drawing context
		window->DC.ColumnsStartX = 0.f + window->WindowPadding.x - window->Scroll.x;
		window->DC.ColumnsOffsetX = 0.f;
		window->DC.CursorStartPos = window->Pos + ImVec2(window->DC.ColumnsStartX + window->DC.ColumnsOffsetX, 
															window->TitleBarHeight() + window->MenuBarHeight() + window->WindowPadding.y - window->Scroll.y);
		window->DC.CursorPos = window->DC.CursorStartPos;
		window->DC.CursorPosPrevLine = window->DC.CursorPos;
		window->DC.CursorMaxPos = window->DC.CursorStartPos;
		window->DC.CurrentLineHeight = window->DC.PrevLineHeight = 0.f;
		window->DC.CurrentLineTextBaseOffset = window->DC.PrevLineTextBaseOffset = 0.f;
		window->DC.MenuBarAppending = false;
		window->DC.MenuBarOffsetX = ImMax(window->WindowPadding.x, style.ItemSpacing.x);
		window->DC.LogLinePosY = window->DC.CursorPos.y - 9999.f;
		window->DC.ChildWindows.resize(0);
		window->DC.LayoutType = ImGuiLayoutType_Vertical;
		window->DC.ItemWidth = window->ItemWidthDefault;
		window->DC.TextWrapPos = -1.f;	//disabled
		window->DC.AllowKeyboardFocus = true;
		window->DC.ButtonRepeat = false;
		window->DC.ItemWidthStack.resize(0);
		window->DC.TextWrapPosStack.resize(0);
		window->DC.AllowKeyboardFocusStack.resize(0);
		window->DC.ButtonRepeatStack.resize(0);
		window->DC.ColorEditMode = ImGuiColorEditMode_UserSelect;
		window->DC.ColumnsCurrent = 0;
		window->DC.ColumnsCount = 1;
		window->DC.ColumnsStartPos = window->DC.CursorPos;
		window->DC.ColumnsCellMinY = window->DC.ColumnsCellMaxY = window->DC.ColumnsStartPos.y;
		window->DC.TreeDepth = 0;
		window->DC.StateStorage = &window->StateStorage;
		window->DC.GroupStack.resize(0);
		window->MenuColumns.Update(3, style.ItemSpacing.x, !windowWasActive);

		if (window->AutoFitFramesX > 0)
			window->AutoFitFramesX--;
		if (window->AutoFitFramesY > 0)
			window->AutoFitFramesY--;

		//Title bar
		if (!(flags & ImGuiWindowFlags_NoTitleBar))
		{
			if (pOpened != NULL)
				CloseWindowButton(pOpened);

			const ImVec2 textSize = CalcTextSize(name, NULL, true);
			if (!(flags & ImGuiWindowFlags_NoCollapse))
				RenderCollapseTriangle(window->Pos + style.FramePadding, !window->Collapsed, 1.f, true);

			ImVec2 textMin = window->Pos + style.FramePadding;
			ImVec2 textMax = window->Pos + ImVec2(window->Size.x - style.FramePadding.x, style.FramePadding.y * 2 + textSize.y);
			ImVec2 clipMax = ImVec2(window->Pos.x + window->Size.x - (pOpened ? titleBarRect.GetHeight() - 3 : style.FramePadding.x), textMax.y);	//Match the size of CloseWindowButton()
			bool padLeft = (flags & ImGuiWindowFlags_NoCollapse) == 0;
			bool padRight = (pOpened != NULL);
			if (style.WindowTitleAlign & ImGuiAlign_Center)
				padRight = padLeft;
			if (padLeft)
				textMin.x += g.FontSize + style.ItemInnerSpacing.x;
			if (padRight)
				textMax.x -= g.FontSize + style.ItemInnerSpacing.x;
			RenderTextClipped(textMin, textMax, name, NULL, &textSize, style.WindowTitleAlign, NULL, &clipMax);
		}

		//Save clipped aabb so we can access it in constant-time in FindHoveredWindow()
		window->ClippedWindowRect = window->Rect();
		window->ClippedWindowRect.Clip(window->ClipRect);

		//Pressing CTRL+C while holding on a window copy its content to the clipboard
		//This works but 1. doesn't handle multiple Begin/End pairs, 2. recursing into another Begin/End pair - so we need to work that out and add better logging scope.
		//Maybe we can support CTRL+C on every element?
		/*
		if (g.ActiveId == move_id)
		{
			if (g.IO.KeyCtrl && IsKeyPressedMap(ImGuiKey_C))
				ImGui::LogToClipboard();
		}
		*/
	}

	//Inner clipping rectangle
	//We set this up after processing the resize grip so that our clip rectangle doesn't lag by a frame
	//Note that if our window is collapsed we will end up with a null clipping rectangle which is the correct behavior.
	const ImRect titleBarRect = window->TitleBarRect();
	ImRect clipRect(titleBarRect.Min.x + 0.5f + window->WindowPadding.x * 0.5f, titleBarRect.Max.y + window->MenuBarHeight() + 0.5f, window->Pos.x + window->Size.x - window->WindowPadding.x * 0.5f, window->Pos.y + window->Size.y);
	if ((flags & ImGuiWindowFlags_ChildWindow) && (flags & ImGuiWindowFlags_ShowBorders))
		clipRect.Min += ImVec2(1.f, 1.f);
	clipRect.Max.x -= window->ScrollbarY ? style.ScrollbarSize : 0.f;
	clipRect.Max.y -= window->ScrollbarX ? style.ScrollbarSize : 0.f;
	PushClipRect(clipRect);

	//Clear 'accessed' flag last thing
	if (firstBeginOfTheFrame)
		window->Accessed = false;
	window->BeginCount++;

	//Child window can be out of sight and have "negative" clip windows.
	//Mark them as collapsed so commands are skipped earlier (we can't manually collapse because they have no title bar).
	if (flags & ImGuiWindowFlags_ChildWindow)
	{
		IM_ASSERT((flags & ImGuiWindowFlags_NoTitleBar) != 0);
		window->Collapsed = parentWindow && parentWindow->Collapsed;

		if (!(flags & ImGuiWindowFlags_AlwaysAutoResize) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
			window->Collapsed |= (window->ClipRect.Min.x >= window->ClipRect.Max.x || window->ClipRect.Min.y >= window->ClipRect.Max.y);

		//We also hide the window from rendering because we've already added its border to the command list.
		//(we could perform the check earlier in the function but it is simpler at this point)
		if (window->Collapsed)
			window->Active = false;
	}
	if (style.Alpha <= 0.f)
		window->Active = false;

	//Return false if we don't intend to display anything to allow user to perform an early out optimization
	window->SkipItems = (window->Collapsed || !window->Active) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0;
	return !window->SkipItems;
}

void ImGui::End()
{
    ImGuiState& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    ImGui::Columns(1, "#CloseColumns");
    PopClipRect();	//inner window clip rectangle

    //Stop logging
    if (!(window->Flags & ImGuiWindowFlags_ChildWindow))	//FIXME: add more options for scope of logging
        ImGui::LogFinish();

    //Pop
    //NB: we don't clear 'window->RootWindow'. The pointer is allowed to live until the next call to Begin().
    g.CurrentWindowStack.pop_back();
    if (window->Flags & ImGuiWindowFlags_Popup)
        g.CurrentPopupStack.pop_back();
    CheckStacksSize(window, false);
    SetCurrentWindow(g.CurrentWindowStack.empty() ? NULL : g.CurrentWindowStack.back());
}

//Vertical scrollbar
//The entire piece of code below is rather confusing because:
//- We handle absolute seeking (when first clicking outside the grab) and relative manipulation (afterward or when clicking inside the grab)
//- We store values as normalized ratio and in a form that allows the window content to change while we are holding on a scrollbar
//- We handle both horizontal and vertical scrollbars, which makes the terminology not ideal.
static void Scrollbar(ImGuiWindow* window, bool horizontal)
{
	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(horizontal ? "#SCROLLX" : "#SCROLLY");

	//Render background
	bool otherScrollbar = (horizontal ? window->ScrollbarY : window->ScrollbarX);
	float otherScrollbarSizeW = otherScrollbar ? style.ScrollbarSize : 0.f;
	const ImRect windowRect = window->Rect();
	const float borderOffset = (window->Flags & ImGuiWindowFlags_ShowBorders) ? 1.f : 0.f;
	ImRect bb = horizontal ? ImRect(window->Pos.x + borderOffset, windowRect.Max.y - style.ScrollbarSize, windowRect.Max.x - otherScrollbarSizeW, windowRect.Max.y)
		: ImRect(windowRect.Max.x - style.ScrollbarSize, window->Pos.y + borderOffset, windowRect.Max.x, windowRect.Max.y - otherScrollbarSizeW);
	if (!horizontal)
		bb.Min.y += window->TitleBarHeight() + ((window->Flags & ImGuiWindowFlags_MenuBar) ? window->MenuBarHeight() - borderOffset : 0.f);

	float windowRounding = (window->Flags & ImGuiWindowFlags_ChildWindow) ? style.ChildWindowRounding : style.WindowRounding;
	int windowRoundingCorners;
	if (horizontal)
		windowRoundingCorners = 8 | (otherScrollbar ? 0 : 4);
	else
		windowRoundingCorners = ((window->Flags & ImGuiWindowFlags_NoTitleBar) ? 2 : 0) | (otherScrollbar ? 0 : 4);
	window->DrawList->AddRectFilled(bb.Min, bb.Max, window->Color(ImGuiCol_ScrollbarBg), windowRounding, windowRoundingCorners);
	bb.Reduce(ImVec2(ImClamp((float)(int)((bb.Max.x - bb.Min.x - 2.f) * 0.5f), 0.f, 3.f), ImClamp((float)(int)((bb.Max.y - bb.Min.y - 2.f) * 0.5f), 0.f, 3.f)));

	//V denote the main axis of the scrollbar
	float scrollbarSizeV = horizontal ? bb.GetWidth() : bb.GetHeight();
	float scrollV = horizontal ? window->Scroll.x : window->Scroll.y;
	float winSizeAvailV = (horizontal ? window->Size.x : window->Size.y) - otherScrollbarSizeW;
	float winSizeContentsV = horizontal ? window->SizeContents.x : window->SizeContents.y;

	//The grabable box size generally represent the amount visible (vs the total scrollable amount)
	//But we maintain a minimum size in pixel to allow for the user to still aim inside.
	const float grabHPixels = ImMin(ImMax(scrollbarSizeV * ImSaturate(winSizeAvailV / ImMax(winSizeContentsV, winSizeAvailV)), style.GrabMinSize), scrollbarSizeV);
	const float grabHNorm = grabHPixels / scrollbarSizeV;

	//Handle input right away. None of the code of Begin() is relying on scrolling position before calling Scrollbar().
	bool held = false;
	bool hovered = false;
	const bool previouslyHeld = (g.ActiveId == id);
	ImGui::ButtonBehavior(bb, id, &hovered, &held);

	float scrollMax = ImMax(1.f, winSizeContentsV - winSizeAvailV);
	float scrollRatio = ImSaturate(scrollV / scrollMax);
	float grabVNorm = scrollRatio * (scrollbarSizeV - grabHPixels) / scrollbarSizeV;
	if (held && grabHNorm < 1.f)
	{
		float scrollbarPosV = horizontal ? bb.Min.x : bb.Min.y;
		float mousePosV = horizontal ? g.IO.MousePos.x : g.IO.MousePos.y;
		float* clickDeltaToGrabCenterV = horizontal ? &g.ScrollbarClickDeltaToGrabCenter.x : &g.ScrollbarClickDeltaToGrabCenter.y;

		//Click position in scrollbar normalized space (0.f->1.f)
		const float clickedVNorm = ImSaturate((mousePosV - scrollbarPosV) / scrollbarSizeV);
		ImGui::SetHoveredID(id);

		bool seekAbsolute = false;
		if (!previouslyHeld)
		{
			//On initial click calculate the distance between mouse and the center of the grab
			if (clickedVNorm >= grabVNorm && clickedVNorm <= grabVNorm + grabHNorm)
				*clickDeltaToGrabCenterV = clickedVNorm - grabVNorm - grabHNorm * 0.5f;
			else
			{
				seekAbsolute = true;
				*clickDeltaToGrabCenterV = 0.f;
			}
		}

		//Apply scroll
		//It is ok to modify Scroll here because we are being called in Begin() after the calculation of SizeContents and before setting up our starting position
		const float scrollVNorm = ImSaturate((clickedVNorm - *clickDeltaToGrabCenterV - grabHNorm * 0.5f) / (1.f - grabHNorm));
		scrollV = (float)(int)(0.5f + scrollVNorm * scrollMax);	//(winSizeContentsV - win_size_v));
		if (horizontal)
			window->Scroll.x = scrollV;
		else
			window->Scroll.y = scrollV;

		//Update values for rendering
		scrollRatio = ImSaturate(scrollV / scrollMax);
		grabVNorm = scrollRatio * (scrollbarSizeV - grabHPixels) / scrollbarSizeV;

		//Update distance to grab now that we have seeked and saturated
		if (seekAbsolute)
			*clickDeltaToGrabCenterV = clickedVNorm - grabVNorm - grabHNorm * 0.5f;
	}

	//Render
	const ImU32 grabCol = window->Color(held ? ImGuiCol_ScrollbarGrabActive : hovered ? ImGuiCol_ScrollbarGrabHovered : ImGuiCol_ScrollbarGrab);
	if (horizontal)
		window->DrawList->AddRectFilled(ImVec2(ImLerp(bb.Min.x, bb.Max.x, grabVNorm), bb.Min.y), ImVec2(ImLerp(bb.Min.x, bb.Max.x, grabVNorm) + grabHPixels, bb.Max.y), grabCol, 
										style.ScrollbarRounding);
	else
		window->DrawList->AddRectFilled(ImVec2(bb.Min.x, ImLerp(bb.Min.y, bb.Max.y, grabVNorm)), ImVec2(bb.Max.x, ImLerp(bb.Min.y, bb.Max.y, grabVNorm) + grabHPixels), grabCol, 
										style.ScrollbarRounding);
}

//Moving window to front of display (which happens to be back of our sorted list)
void ImGui::FocusWindow(ImGuiWindow* window)
{
	ImGuiState& g = *GImGui;

	//Always mark the window we passed as focused. This is used for keyboard interactions such as tabbing.
	g.FocusedWindow = window;

	//Passing NULL allow to disable keyboard focus
	if (!window)
		return;

	//And move its root window to the top of the pile
	if (window->RootWindow)
		window = window->RootWindow;

	//Steal focus on active widgets
	if (window->Flags & ImGuiWindowFlags_Popup)	//FIXME: This statement should be unnecessary. Need further testing before removing it..
	{
		if (g.ActiveId != 0 && g.ActiveIdWindow && g.ActiveIdWindow->RootWindow != window)
			ImGui::SetActiveID(0);
	}

	//Bring to front
	if ((window->Flags & ImGuiWindowFlags_NoBringToFrontOnFocus) || g.Windows.back() == window)
		return;
	for (int i = 0; i < g.Windows.Size; i++)
	{
		if (g.Windows[i] == window)
		{
			g.Windows.erase(g.Windows.begin() + i);
			break;
		}
	}
	g.Windows.push_back(window);
}

void ImGui::PushItemWidth(float itemWidth)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.ItemWidth = (itemWidth == 0.f ? window->ItemWidthDefault : itemWidth);
	window->DC.ItemWidthStack.push_back(window->DC.ItemWidth);
}

static void PushMultiItemsWidths(int components, float wFull)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	const ImGuiStyle& style = GImGui->Style;
	if (wFull <= 0.f)
		wFull = ImGui::CalcItemWidth();
	const float wItemOne = ImMax(1.f, (float)(int)((wFull - (style.FramePadding.x * 2.f + style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
	const float wItemLast = ImMax(1.f, (float)(int)(wFull - (wItemOne + style.FramePadding.x * 2.f + style.ItemInnerSpacing.x) * (components - 1)));
	window->DC.ItemWidthStack.push_back(wItemLast);
	for (int i = 0; i < components - 1; i++)
		window->DC.ItemWidthStack.push_back(wItemOne);
	window->DC.ItemWidth = window->DC.ItemWidthStack.back();
}

void ImGui::PopItemWidth()
{
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.ItemWidthStack.pop_back();
	window->DC.ItemWidth = window->DC.ItemWidthStack.empty() ? window->ItemWidthDefault : window->DC.ItemWidthStack.back();
}

float ImGui::CalcItemWidth()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	float w = window->DC.ItemWidth;
	if (w < 0.f)
	{
		//Align to a right-side limit. We include 1 frame padding in the calculation because this is how the width is always used (we add 2 frame padding to it), but we could move that responsibility to the widget as well.
		ImGuiState& g = *GImGui;
		float widthToRightEdge = ImGui::GetContentRegionAvail().x;
		w = ImMax(1.f, widthToRightEdge + w - g.Style.FramePadding.x * 2.f);
	}
	w = (float)(int)w;
	return w;
}

static void SetCurrentFont(ImFont* font)
{
	ImGuiState& g = *GImGui;
	IM_ASSERT(font && font->IsLoaded());
	IM_ASSERT(font->Scale > 0.f);
	g.Font = font;
	g.FontBaseSize = g.IO.FontGlobalScale * g.Font->FontSize * g.Font->Scale;
	g.FontSize = g.CurrentWindow ? g.CurrentWindow->CalcFontSize() : 0.f;
	g.FontTexUvWhitePixel = g.Font->ContainerAtlas->TexUvWhitePixel;
}

void ImGui::PushFont(ImFont* font)
{
	ImGuiState& g = *GImGui;
	if (!font)
		font = g.IO.Fonts->Fonts[0];
	SetCurrentFont(font);
	g.FontStack.push_back(font);
	g.CurrentWindow->DrawList->PushTextureID(font->ContainerAtlas->TexID);
}

void  ImGui::PopFont()
{
	ImGuiState& g = *GImGui;
	g.CurrentWindow->DrawList->PopTextureID();
	g.FontStack.pop_back();
	SetCurrentFont(g.FontStack.empty() ? g.IO.Fonts->Fonts[0] : g.FontStack.back());
}

void ImGui::PushAllowKeyboardFocus(bool allowKeyboardFocus)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.AllowKeyboardFocus = allowKeyboardFocus;
	window->DC.AllowKeyboardFocusStack.push_back(allowKeyboardFocus);
}

void ImGui::PopAllowKeyboardFocus()
{
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.AllowKeyboardFocusStack.pop_back();
	window->DC.AllowKeyboardFocus = window->DC.AllowKeyboardFocusStack.empty() ? true : window->DC.AllowKeyboardFocusStack.back();
}

void ImGui::PushButtonRepeat(bool repeat)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.ButtonRepeat = repeat;
	window->DC.ButtonRepeatStack.push_back(repeat);
}

void ImGui::PopButtonRepeat()
{
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.ButtonRepeatStack.pop_back();
	window->DC.ButtonRepeat = window->DC.ButtonRepeatStack.empty() ? false : window->DC.ButtonRepeatStack.back();
}

void ImGui::PushTextWrapPos(float wrapPosX)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.TextWrapPos = wrapPosX;
	window->DC.TextWrapPosStack.push_back(wrapPosX);
}

void ImGui::PopTextWrapPos()
{
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.TextWrapPosStack.pop_back();
	window->DC.TextWrapPos = window->DC.TextWrapPosStack.empty() ? -1.f : window->DC.TextWrapPosStack.back();
}

void ImGui::PushStyleColor(ImGuiCol idx, const ImVec4& col)
{
	ImGuiState& g = *GImGui;
	ImGuiColMod backup;
	backup.Col = idx;
	backup.PreviousValue = g.Style.Colors[idx];
	g.ColorModifiers.push_back(backup);
	g.Style.Colors[idx] = col;
}

void ImGui::PopStyleColor(int count)
{
	ImGuiState& g = *GImGui;
	while (count > 0)
	{
		ImGuiColMod& backup = g.ColorModifiers.back();
		g.Style.Colors[backup.Col] = backup.PreviousValue;
		g.ColorModifiers.pop_back();
		count--;
	}
}

static float* GetStyleVarFloatAddr(ImGuiStyleVar idx)
{
	ImGuiState& g = *GImGui;
	switch (idx)
	{
	case ImGuiStyleVar_Alpha: return &g.Style.Alpha;
	case ImGuiStyleVar_WindowRounding: return &g.Style.WindowRounding;
	case ImGuiStyleVar_ChildWindowRounding: return &g.Style.ChildWindowRounding;
	case ImGuiStyleVar_FrameRounding: return &g.Style.FrameRounding;
	case ImGuiStyleVar_IndentSpacing: return &g.Style.IndentSpacing;
	case ImGuiStyleVar_GrabMinSize: return &g.Style.GrabMinSize;
	}
	return NULL;
}

static ImVec2* GetStyleVarVec2Addr(ImGuiStyleVar idx)
{
	ImGuiState& g = *GImGui;
	switch (idx)
	{
	case ImGuiStyleVar_WindowPadding: return &g.Style.WindowPadding;
	case ImGuiStyleVar_WindowMinSize: return &g.Style.WindowMinSize;
	case ImGuiStyleVar_FramePadding: return &g.Style.FramePadding;
	case ImGuiStyleVar_ItemSpacing: return &g.Style.ItemSpacing;
	case ImGuiStyleVar_ItemInnerSpacing: return &g.Style.ItemInnerSpacing;
	}
	return NULL;
}

void ImGui::PushStyleVar(ImGuiStyleVar idx, float val)
{
	ImGuiState& g = *GImGui;
	float* pvar = GetStyleVarFloatAddr(idx);
	IM_ASSERT(pvar != NULL);	//Called function with wrong-type? Variable is not a float.
	ImGuiStyleMod backup;
	backup.Var = idx;
	backup.PreviousValue = ImVec2(*pvar, 0.f);
	g.StyleModifiers.push_back(backup);
	*pvar = val;
}

void ImGui::PushStyleVar(ImGuiStyleVar idx, const ImVec2& val)
{
	ImGuiState& g = *GImGui;
	ImVec2* pvar = GetStyleVarVec2Addr(idx);
	IM_ASSERT(pvar != NULL);	//Called function with wrong-type? Variable is not a ImVec2.
	ImGuiStyleMod backup;
	backup.Var = idx;
	backup.PreviousValue = *pvar;
	g.StyleModifiers.push_back(backup);
	*pvar = val;
}

void ImGui::PopStyleVar(int count)
{
	ImGuiState& g = *GImGui;
	while (count > 0)
	{
		ImGuiStyleMod& backup = g.StyleModifiers.back();
		if (float* pvarF = GetStyleVarFloatAddr(backup.Var))
			*pvarF = backup.PreviousValue.x;
		else if (ImVec2* pvarV = GetStyleVarVec2Addr(backup.Var))
			*pvarV = backup.PreviousValue;
		g.StyleModifiers.pop_back();
		count--;
	}
}

const char* ImGui::GetStyleColName(ImGuiCol idx)
{
	//Create switch-case from enum with regexp: ImGuiCol_{.*}, --> case ImGuiCol_\1: return "\1";
	switch (idx)
	{
	case ImGuiCol_Text: return "Text";
	case ImGuiCol_TextDisabled: return "TextDisabled";
	case ImGuiCol_WindowBg: return "WindowBg";
	case ImGuiCol_ChildWindowBg: return "ChildWindowBg";
	case ImGuiCol_Border: return "Border";
	case ImGuiCol_BorderShadow: return "BorderShadow";
	case ImGuiCol_FrameBg: return "FrameBg";
	case ImGuiCol_FrameBgHovered: return "FrameBgHovered";
	case ImGuiCol_FrameBgActive: return "FrameBgActive";
	case ImGuiCol_TitleBg: return "TitleBg";
	case ImGuiCol_TitleBgCollapsed: return "TitleBgCollapsed";
	case ImGuiCol_TitleBgActive: return "TitleBgActive";
	case ImGuiCol_MenuBarBg: return "MenuBarBg";
	case ImGuiCol_ScrollbarBg: return "ScrollbarBg";
	case ImGuiCol_ScrollbarGrab: return "ScrollbarGrab";
	case ImGuiCol_ScrollbarGrabHovered: return "ScrollbarGrabHovered";
	case ImGuiCol_ScrollbarGrabActive: return "ScrollbarGrabActive";
	case ImGuiCol_ComboBg: return "ComboBg";
	case ImGuiCol_CheckMark: return "CheckMark";
	case ImGuiCol_SliderGrab: return "SliderGrab";
	case ImGuiCol_SliderGrabActive: return "SliderGrabActive";
	case ImGuiCol_Button: return "Button";
	case ImGuiCol_ButtonHovered: return "ButtonHovered";
	case ImGuiCol_ButtonActive: return "ButtonActive";
	case ImGuiCol_Header: return "Header";
	case ImGuiCol_HeaderHovered: return "HeaderHovered";
	case ImGuiCol_HeaderActive: return "HeaderActive";
	case ImGuiCol_Column: return "Column";
	case ImGuiCol_ColumnHovered: return "ColumnHovered";
	case ImGuiCol_ColumnActive: return "ColumnActive";
	case ImGuiCol_ResizeGrip: return "ResizeGrip";
	case ImGuiCol_ResizeGripHovered: return "ResizeGripHovered";
	case ImGuiCol_ResizeGripActive: return "ResizeGripActive";
	case ImGuiCol_CloseButton: return "CloseButton";
	case ImGuiCol_CloseButtonHovered: return "CloseButtonHovered";
	case ImGuiCol_CloseButtonActive: return "CloseButtonActive";
	case ImGuiCol_PlotLines: return "PlotLines";
	case ImGuiCol_PlotLinesHovered: return "PlotLinesHovered";
	case ImGuiCol_PlotHistogram: return "PlotHistogram";
	case ImGuiCol_PlotHistogramHovered: return "PlotHistogramHovered";
	case ImGuiCol_TextSelectedBg: return "TextSelectedBg";
	case ImGuiCol_TooltipBg: return "TooltipBg";
	case ImGuiCol_ModalWindowDarkening: return "ModalWindowDarkening";
	}
	IM_ASSERT(0);
	return "Unknown";
}

bool ImGui::IsWindowHovered()
{
	ImGuiState& g = *GImGui;
	return g.HoveredWindow == g.CurrentWindow && IsWindowContentHoverable(g.HoveredRootWindow);
}

bool ImGui::IsWindowFocused()
{
	ImGuiState& g = *GImGui;
	return g.FocusedWindow == g.CurrentWindow;
}

bool ImGui::IsRootWindowFocused()
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* rootWindow = g.CurrentWindow->RootWindow;
	return g.FocusedWindow == rootWindow;
}

bool ImGui::IsRootWindowOrAnyChildFocused()
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* rootWindow = g.CurrentWindow->RootWindow;
	return g.FocusedWindow && g.FocusedWindow->RootWindow == rootWindow;
}

float ImGui::GetWindowWidth()
{
	ImGuiWindow* window = GImGui->CurrentWindow;
	return window->Size.x;
}

float ImGui::GetWindowHeight()
{
	ImGuiWindow* window = GImGui->CurrentWindow;
	return window->Size.y;
}

ImVec2 ImGui::GetWindowPos()
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	return window->Pos;
}

static void SetWindowScrollY(ImGuiWindow* window, float newScrollY)
{
	window->DC.CursorMaxPos.y += window->Scroll.y;
	window->Scroll.y = newScrollY;
	window->DC.CursorMaxPos.y -= window->Scroll.y;
}

static void SetWindowPos(ImGuiWindow* window, const ImVec2& pos, ImGuiSetCond cond)
{
	//Test condition (NB: bit 0 is always true) and clear flags for next time
	if (cond && (window->SetWindowPosAllowFlags & cond) == 0)
		return;
	window->SetWindowPosAllowFlags &= ~(ImGuiSetCond_Once | ImGuiSetCond_FirstUseEver | ImGuiSetCond_Appearing);
	window->SetWindowPosCenterWanted = false;

	//Set
	const ImVec2 oldPos = window->Pos;
	window->PosFloat = pos;
	window->Pos = ImVec2((float)(int)window->PosFloat.x, (float)(int)window->PosFloat.y);
	window->DC.CursorPos += (window->Pos - oldPos);	//As we happen to move the window while it is being appended to (which is a bad idea - will smear) let's at least offset the cursor
	window->DC.CursorMaxPos += (window->Pos - oldPos);	//And more importantly we need to adjust this so size calculation doesn't get affected.
}

void ImGui::SetWindowPos(const ImVec2& pos, ImGuiSetCond cond)
{
	ImGuiWindow* window = GetCurrentWindow();
	SetWindowPos(window, pos, cond);
}

void ImGui::SetWindowPos(const char* name, const ImVec2& pos, ImGuiSetCond cond)
{
	ImGuiWindow* window = FindWindowByName(name);
	if (window)
		SetWindowPos(window, pos, cond);
}

ImVec2 ImGui::GetWindowSize()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return window->Size;
}

static void SetWindowSize(ImGuiWindow* window, const ImVec2& size, ImGuiSetCond cond)
{
	//Test condition (NB: bit 0 is always true) and clear flags for next time
	if (cond && (window->SetWindowSizeAllowFlags & cond) == 0)
		return;
	window->SetWindowSizeAllowFlags &= ~(ImGuiSetCond_Once | ImGuiSetCond_FirstUseEver | ImGuiSetCond_Appearing);

	//Set
	if (size.x > 0.f)
	{
		window->AutoFitFramesX = 0;
		window->SizeFull.x = size.x;
	}
	else
	{
		window->AutoFitFramesX = 2;
		window->AutoFitOnlyGrows = false;
	}
	if (size.y > 0.f)
	{
		window->AutoFitFramesY = 0;
		window->SizeFull.y = size.y;
	}
	else
	{
		window->AutoFitFramesY = 2;
		window->AutoFitOnlyGrows = false;
	}
}

void ImGui::SetWindowSize(const ImVec2& size, ImGuiSetCond cond) { 	SetWindowSize(GImGui->CurrentWindow, size, cond); }

void ImGui::SetWindowSize(const char* name, const ImVec2& size, ImGuiSetCond cond)
{
	ImGuiWindow* window = FindWindowByName(name);
	if (window)
		SetWindowSize(window, size, cond);
}

static void SetWindowCollapsed(ImGuiWindow* window, bool collapsed, ImGuiSetCond cond)
{
	//Test condition (NB: bit 0 is always true) and clear flags for next time
	if (cond && (window->SetWindowCollapsedAllowFlags & cond) == 0)
		return;
	window->SetWindowCollapsedAllowFlags &= ~(ImGuiSetCond_Once | ImGuiSetCond_FirstUseEver | ImGuiSetCond_Appearing);

	//Set
	window->Collapsed = collapsed;
}

void ImGui::SetWindowCollapsed(bool collapsed, ImGuiSetCond cond) { SetWindowCollapsed(GImGui->CurrentWindow, collapsed, cond); }

bool ImGui::IsWindowCollapsed() { return GImGui->CurrentWindow->Collapsed; }

void ImGui::SetWindowCollapsed(const char* name, bool collapsed, ImGuiSetCond cond)
{
	ImGuiWindow* window = FindWindowByName(name);
	if (window)
		SetWindowCollapsed(window, collapsed, cond);
}

void ImGui::SetWindowFocus() { FocusWindow(GImGui->CurrentWindow); }

void ImGui::SetWindowFocus(const char* name)
{
	if (name)
	{
		ImGuiWindow* window = FindWindowByName(name);
		if (window)
			FocusWindow(window);
	}
	else
		FocusWindow(NULL);
}

void ImGui::SetNextWindowPos(const ImVec2& pos, ImGuiSetCond cond)
{
	ImGuiState& g = *GImGui;
	g.SetNextWindowPosVal = pos;
	g.SetNextWindowPosCond = cond ? cond : ImGuiSetCond_Always;
}

void ImGui::SetNextWindowPosCenter(ImGuiSetCond cond)
{
	ImGuiState& g = *GImGui;
	g.SetNextWindowPosVal = ImVec2(-FLT_MAX, -FLT_MAX);
	g.SetNextWindowPosCond = cond ? cond : ImGuiSetCond_Always;
}

void ImGui::SetNextWindowSize(const ImVec2& size, ImGuiSetCond cond)
{
	ImGuiState& g = *GImGui;
	g.SetNextWindowSizeVal = size;
	g.SetNextWindowSizeCond = cond ? cond : ImGuiSetCond_Always;
}

void ImGui::SetNextWindowContentSize(const ImVec2& size)
{
	ImGuiState& g = *GImGui;
	g.SetNextWindowContentSizeVal = size;
	g.SetNextWindowContentSizeCond = ImGuiSetCond_Always;
}

void ImGui::SetNextWindowContentWidth(float width)
{
	ImGuiState& g = *GImGui;
	g.SetNextWindowContentSizeVal = ImVec2(width, g.SetNextWindowContentSizeCond ? g.SetNextWindowContentSizeVal.y : 0.f);
	g.SetNextWindowContentSizeCond = ImGuiSetCond_Always;
}

void ImGui::SetNextWindowCollapsed(bool collapsed, ImGuiSetCond cond)
{
	ImGuiState& g = *GImGui;
	g.SetNextWindowCollapsedVal = collapsed;
	g.SetNextWindowCollapsedCond = cond ? cond : ImGuiSetCond_Always;
}

void ImGui::SetNextWindowFocus()
{
	ImGuiState& g = *GImGui;
	g.SetNextWindowFocus = true;
}

//In window space (not screen space!)
ImVec2 ImGui::GetContentRegionMax()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	ImVec2 contentRegionSize = ImVec2(window->SizeContentsExplicit.x ? window->SizeContentsExplicit.x : window->Size.x - window->ScrollbarSizes.x, 
										window->SizeContentsExplicit.y ? window->SizeContentsExplicit.y : window->Size.y - window->ScrollbarSizes.y);
	ImVec2 mx = contentRegionSize - window->Scroll - window->WindowPadding;
	if (window->DC.ColumnsCount != 1)
		mx.x = ImGui::GetColumnOffset(window->DC.ColumnsCurrent + 1) - window->WindowPadding.x;
	return mx;
}

ImVec2 ImGui::GetContentRegionAvail()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return GetContentRegionMax() - (window->DC.CursorPos - window->Pos);
}

float ImGui::GetContentRegionAvailWidth() { return GetContentRegionAvail().x; }

//In window space (not screen space!)
ImVec2 ImGui::GetWindowContentRegionMin()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return ImVec2(-window->Scroll.x, -window->Scroll.y + window->TitleBarHeight() + window->MenuBarHeight()) + window->WindowPadding;
}

ImVec2 ImGui::GetWindowContentRegionMax()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	ImVec2 contentRegionSize = ImVec2(window->SizeContentsExplicit.x ? window->SizeContentsExplicit.x : window->Size.x, 
										window->SizeContentsExplicit.y ? window->SizeContentsExplicit.y : window->Size.y);
	ImVec2 m = contentRegionSize - window->Scroll - window->WindowPadding - window->ScrollbarSizes;
	return m;
}

float ImGui::GetWindowContentRegionWidth() { return GetWindowContentRegionMax().x - GetWindowContentRegionMin().x; }

float ImGui::GetTextLineHeight()
{
    ImGuiState& g = *GImGui;
    return g.FontSize;
}

float ImGui::GetTextLineHeightWithSpacing()
{
	ImGuiState& g = *GImGui;
	return g.FontSize + g.Style.ItemSpacing.y;
}

float ImGui::GetItemsLineHeightWithSpacing()
{
	ImGuiState& g = *GImGui;
	return g.FontSize + g.Style.FramePadding.y * 2.f + g.Style.ItemSpacing.y;
}

ImDrawList* ImGui::GetWindowDrawList()
{
	ImGuiWindow* window = GetCurrentWindow();
	return window->DrawList;
}

ImFont* ImGui::GetWindowFont()
{
	ImGuiState& g = *GImGui;
	return g.Font;
}

float ImGui::GetWindowFontSize()
{
	ImGuiState& g = *GImGui;
	return g.FontSize;
}

void ImGui::SetWindowFontScale(float scale)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();
	window->FontWindowScale = scale;
	g.FontSize = window->CalcFontSize();
}

//User generally sees positions in window coordinates. Internally we store CursorPos in absolute screen coordinates because it is more convenient.
//Conversion happens as we pass the value to user, but it makes our naming convention confusing because GetCursorPos() == (DC.CursorPos - window.Pos). May want to rename 'DC.CursorPos'.
ImVec2 ImGui::GetCursorPos()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return window->DC.CursorPos - window->Pos + window->Scroll;
}

float ImGui::GetCursorPosX()
{
	ImGuiWindow* window = GetCurrentWindow();
	return window->DC.CursorPos.x - window->Pos.x + window->Scroll.x;
}

float ImGui::GetCursorPosY()
{
	ImGuiWindow* window = GetCurrentWindow();
	return window->DC.CursorPos.y - window->Pos.y + window->Scroll.y;
}

void ImGui::SetCursorPos(const ImVec2& local_pos)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.CursorPos = window->Pos - window->Scroll + local_pos;
	window->DC.CursorMaxPos = ImMax(window->DC.CursorMaxPos, window->DC.CursorPos);
}

void ImGui::SetCursorPosX(float x)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.CursorPos.x = window->Pos.x - window->Scroll.x + x;
	window->DC.CursorMaxPos.x = ImMax(window->DC.CursorMaxPos.x, window->DC.CursorPos.x);
}

void ImGui::SetCursorPosY(float y)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.CursorPos.y = window->Pos.y - window->Scroll.y + y;
	window->DC.CursorMaxPos.y = ImMax(window->DC.CursorMaxPos.y, window->DC.CursorPos.y);
}

ImVec2 ImGui::GetCursorStartPos()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return window->DC.CursorStartPos - window->Pos;
}

ImVec2 ImGui::GetCursorScreenPos()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return window->DC.CursorPos;
}

void ImGui::SetCursorScreenPos(const ImVec2& screenPos)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.CursorPos = screenPos;
}

float ImGui::GetScrollX() { return GImGui->CurrentWindow->Scroll.x; }

float ImGui::GetScrollY() { return GImGui->CurrentWindow->Scroll.y; }

float ImGui::GetScrollMaxX()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return window->SizeContents.x - window->SizeFull.x - window->ScrollbarSizes.x;
}

float ImGui::GetScrollMaxY()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return window->SizeContents.y - window->SizeFull.y - window->ScrollbarSizes.y;
}

void ImGui::SetScrollX(float scrollX)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->ScrollTarget.x = scrollX;
	window->ScrollTargetCenterRatio.x = 0.f;
}

void ImGui::SetScrollY(float scrollY)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->ScrollTarget.y = scrollY + window->TitleBarHeight();	//title bar height canceled out when using ScrollTargetRelY
	window->ScrollTargetCenterRatio.y = 0.f;
}

void ImGui::SetScrollFromPosY(float posY, float centerYRatio)
{
	//We store a target position so centering can occur on the next frame when we are guaranteed to have a known window size
	ImGuiWindow* window = GetCurrentWindow();
	IM_ASSERT(centerYRatio >= 0.f && centerYRatio <= 1.f);
	window->ScrollTarget.y = (float)(int)(posY + window->Scroll.y);
	if (centerYRatio <= 0.f && window->ScrollTarget.y <= window->WindowPadding.y)	//Minor hack to make "scroll to top" take account of WindowPadding, else it would scroll to (WindowPadding.y - ItemSpacing.y)
		window->ScrollTarget.y = 0.f;
	window->ScrollTargetCenterRatio.y = centerYRatio;
}

//centerYRatio: 0.f top of last item, 0.5f vertical center of last item, 1.f bottom of last item.
void ImGui::SetScrollHere(float centerYRatio)
{
	ImGuiWindow* window = GetCurrentWindow();
	float targetY = window->DC.CursorPosPrevLine.y + (window->DC.PrevLineHeight * centerYRatio) + (GImGui->Style.ItemSpacing.y * (centerYRatio - 0.5f) * 2.f);	//Precisely aim above, in the middle or below the last line.
	ImGui::SetScrollFromPosY(targetY - window->Pos.y, centerYRatio);
}

void ImGui::SetKeyboardFocusHere(int offset)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->FocusIdxAllRequestNext = window->FocusIdxAllCounter + 1 + offset;
	window->FocusIdxTabRequestNext = IM_INT_MAX;
}

void ImGui::SetStateStorage(ImGuiStorage* tree)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.StateStorage = tree ? tree : &window->StateStorage;
}

ImGuiStorage* ImGui::GetStateStorage()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return window->DC.StateStorage;
}

void ImGui::TextV(const char* fmt, va_list args)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiState& g = *GImGui;
	const char* textEnd = g.TempBuffer + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
	TextUnformatted(g.TempBuffer, textEnd);
}

void ImGui::Text(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	TextV(fmt, args);
	va_end(args);
}

void ImGui::TextColoredV(const ImVec4& col, const char* fmt, va_list args)
{
	ImGui::PushStyleColor(ImGuiCol_Text, col);
	TextV(fmt, args);
	ImGui::PopStyleColor();
}

void ImGui::TextColored(const ImVec4& col, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	TextColoredV(col, fmt, args);
	va_end(args);
}

void ImGui::TextDisabledV(const char* fmt, va_list args)
{
	ImGui::PushStyleColor(ImGuiCol_Text, GImGui->Style.Colors[ImGuiCol_TextDisabled]);
	TextV(fmt, args);
	ImGui::PopStyleColor();
}

void ImGui::TextDisabled(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	TextDisabledV(fmt, args);
	va_end(args);
}

void ImGui::TextWrappedV(const char* fmt, va_list args)
{
	ImGui::PushTextWrapPos(0.f);
	TextV(fmt, args);
	ImGui::PopTextWrapPos();
}

void ImGui::TextWrapped(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	TextWrappedV(fmt, args);
	va_end(args);
}

void ImGui::TextUnformatted(const char* text, const char* textEnd)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiState& g = *GImGui;
	IM_ASSERT(text != NULL);
	const char* textBegin = text;
	if (textEnd == NULL)
		textEnd = text + strlen(text);	//FIXME-OPT

	const float wrapPosX = window->DC.TextWrapPos;
	const bool wrapEnabled = wrapPosX >= 0.f;
	if (textEnd - text > 2000 && !wrapEnabled)
	{
		//Long text!
		//Perform manual coarse clipping to optimize for long multi-line text
		//From this point we will only compute the width of lines that are visible. Optimization only available when word-wrapping is disabled.
		//We also don't vertically center the text within the line full height, which is unlikely to matter because we are likely the biggest and only item on the line.
		const char* line = text;
		const float lineHeight = ImGui::GetTextLineHeight();
		const ImVec2 textPos = window->DC.CursorPos + ImVec2(0.f, window->DC.CurrentLineTextBaseOffset);
		const ImRect clipRect = window->ClipRect;
		ImVec2 textSize(0, 0);

		if (textPos.y <= clipRect.Max.y)
		{
			ImVec2 pos = textPos;

			//Lines to skip (can't skip when logging text)
			if (!g.LogEnabled)
			{
				int linesSkippable = (int)((clipRect.Min.y - textPos.y) / lineHeight);
				if (linesSkippable > 0)
				{
					int linesSkipped = 0;
					while (line < textEnd && linesSkipped < linesSkippable)
					{
						const char* lineEnd = strchr(line, '\n');
						line = lineEnd + 1;
						linesSkipped++;
					}
					pos.y += linesSkipped * lineHeight;
				}
			}

			//Lines to render
			if (line < textEnd)
			{
				ImRect lineRect(pos, pos + ImVec2(ImGui::GetWindowWidth(), lineHeight));
				while (line < textEnd)
				{
					const char* lineEnd = strchr(line, '\n');
					if (IsClippedEx(lineRect, NULL, false))
						break;

					const ImVec2 lineSize = CalcTextSize(line, lineEnd, false);
					textSize.x = ImMax(textSize.x, lineSize.x);
					RenderText(pos, line, lineEnd, false);
					if (!lineEnd)
						lineEnd = textEnd;
					line = lineEnd + 1;
					lineRect.Min.y += lineHeight;
					lineRect.Max.y += lineHeight;
					pos.y += lineHeight;
				}

				//Count remaining lines
				int linesSkipped = 0;
				while (line < textEnd)
				{
					const char* lineEnd = strchr(line, '\n');
					if (!lineEnd)
						lineEnd = textEnd;
					line = lineEnd + 1;
					linesSkipped++;
				}
				pos.y += linesSkipped * lineHeight;
			}

			textSize.y += (pos - textPos).y;
		}

		ImRect bb(textPos, textPos + textSize);
		ItemSize(bb);
		ItemAdd(bb, NULL);
	}
	else
	{
		const float wrapWidth = wrapEnabled ? CalcWrapWidthForPos(window->DC.CursorPos, wrapPosX) : 0.f;
		const ImVec2 textSize = CalcTextSize(textBegin, textEnd, false, wrapWidth);

		//Account of baseline offset
		ImVec2 textPos = window->DC.CursorPos;
		textPos.y += window->DC.CurrentLineTextBaseOffset;

		ImRect bb(textPos, textPos + textSize);
		ItemSize(textSize);
		if (!ItemAdd(bb, NULL))
			return;

		//Render (we don't hide text after ## in this end-user function)
		RenderTextWrapped(bb.Min, textBegin, textEnd, wrapWidth);
	}
}

void ImGui::AlignFirstTextHeightToWidgets()
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	//Declare a dummy item size to that upcoming items that are smaller will center-align on the newly expanded line height.
	ImGuiState& g = *GImGui;
	ItemSize(ImVec2(0, g.FontSize + g.Style.FramePadding.y * 2), g.Style.FramePadding.y);
	ImGui::SameLine(0, 0);
}

//Add a label+text combo aligned to other label+value widgets
void ImGui::LabelTextV(const char* label, const char* fmt, va_list args)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const float w = CalcItemWidth();

	const ImVec2 labelSize = CalcTextSize(label, NULL, true);
	const ImRect valueBB(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w + style.FramePadding.x * 2, labelSize.y + style.FramePadding.y * 2));
	const ImRect totalBB(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w + style.FramePadding.x * 2 + (labelSize.x > 0.f ? style.ItemInnerSpacing.x : 0.f), 
																				style.FramePadding.y * 2) + labelSize);
	ItemSize(totalBB, style.FramePadding.y);
	if (!ItemAdd(totalBB, NULL))
		return;

	//Render
	const char* valueTextBegin = &g.TempBuffer[0];
	const char* valueTextEnd = valueTextBegin + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
	RenderTextClipped(valueBB.Min, valueBB.Max, valueTextBegin, valueTextEnd, NULL, ImGuiAlign_VCenter);
	RenderText(ImVec2(valueBB.Max.x + style.ItemInnerSpacing.x, valueBB.Min.y + style.FramePadding.y), label);
}

void ImGui::LabelText(const char* label, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	LabelTextV(label, fmt, args);
	va_end(args);
}

static inline bool IsWindowContentHoverable(ImGuiWindow* window)
{
	//An active popup disable hovering on other windows (apart from its own children)
	ImGuiState& g = *GImGui;
	if (ImGuiWindow* focusedWindow = g.FocusedWindow)
	{
		if (ImGuiWindow* focusedRootWindow = focusedWindow->RootWindow)
		{
			if ((focusedRootWindow->Flags & ImGuiWindowFlags_Popup) != 0 && focusedRootWindow->WasActive && focusedRootWindow != window->RootWindow)
				return false;
		}
	}

	return true;
}

bool ImGui::ButtonBehavior(const ImRect& bb, ImGuiID id, bool* outHovered, bool* outHeld, ImGuiButtonFlags flags)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();

	if (flags & ImGuiButtonFlags_Disabled)
	{
		if (outHovered)
			*outHovered = false;
		if (outHeld)
			*outHeld = false;
		if (g.ActiveId == id)
			SetActiveID(0);
		return false;
	}

	bool pressed = false;
	const bool hovered = IsHovered(bb, id, (flags & ImGuiButtonFlags_FlattenChilds) != 0);
	if (hovered)
	{
		SetHoveredID(id);
		if (!(flags & ImGuiButtonFlags_NoKeyModifiers) || (!g.IO.KeyCtrl && !g.IO.KeyShift && !g.IO.KeyAlt))
		{
			if (g.IO.MouseClicked[0])
			{
				if (flags & ImGuiButtonFlags_PressedOnClick)
				{
					pressed = true;
					SetActiveID(0);
				}
				else
					SetActiveID(id, window);
				FocusWindow(window);
			}
			else if (g.IO.MouseReleased[0] && (flags & ImGuiButtonFlags_PressedOnRelease))
			{
				pressed = true;
				SetActiveID(0);
			}
			else if ((flags & ImGuiButtonFlags_Repeat) && g.ActiveId == id && ImGui::IsMouseClicked(0, true))
				pressed = true;
		}
	}

	bool held = false;
	if (g.ActiveId == id)
	{
		if (g.IO.MouseDown[0])
			held = true;
		else
		{
			if (hovered)
				pressed = true;
			SetActiveID(0);
		}
	}

	if (outHovered)
		*outHovered = hovered;
	if (outHeld)
		*outHeld = held;

	return pressed;
}

bool ImGui::ButtonEx(const char* label, const ImVec2& sizeArg, ImGuiButtonFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 labelSize = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = CalcItemSize(sizeArg, labelSize.x + style.FramePadding.x * 2.f, labelSize.y + style.FramePadding.y * 2.f);

	const ImRect bb(pos, pos + size);
	ItemSize(bb, style.FramePadding.y);
	if (!ItemAdd(bb, &id))
		return false;

	if (window->DC.ButtonRepeat)
		flags |= ImGuiButtonFlags_Repeat;
	bool hovered;
	bool held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	//Render
	const ImU32 col = window->Color((hovered && held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
	RenderTextClipped(bb.Min, bb.Max, label, NULL, &labelSize, ImGuiAlign_Center | ImGuiAlign_VCenter);

	//Automatically close popups
	//if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
	//   ImGui::CloseCurrentPopup();

	return pressed;
}

bool ImGui::Button(const char* label, const ImVec2& sizeArg) { return ButtonEx(label, sizeArg, 0); }

//Small buttons fits within text without additional vertical spacing.
bool ImGui::SmallButton(const char* label)
{
	ImGuiState& g = *GImGui;
	float backupPaddingY = g.Style.FramePadding.y;
	g.Style.FramePadding.y = 0.f;
	bool pressed = ButtonEx(label, ImVec2(0, 0), ImGuiButtonFlags_AlignTextBaseLine);
	g.Style.FramePadding.y = backupPaddingY;
	return pressed;
}

//Tip: use ImGui::PushID()/PopID() to push indices or pointers in the ID stack.
//Then you can keep 'strID' empty or the same for all your buttons (instead of creating a string based on a non-string id)
bool ImGui::InvisibleButton(const char* strID, const ImVec2& sizeArg)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	const ImGuiID id = window->GetID(strID);
	ImVec2 size = CalcItemSize(sizeArg, 0.f, 0.f);
	const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
	ItemSize(bb);
	if (!ItemAdd(bb, &id))
		return false;

	bool hovered;
	bool held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held);

	return pressed;
}

//Upper-right button to close a window.
static bool CloseWindowButton(bool* pOpened)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	const ImGuiID id = window->GetID("#CLOSE");
	const float size = window->TitleBarHeight() - 4.f;
	const ImRect bb(window->Rect().GetTR() + ImVec2(-2.f - size, 2.f), window->Rect().GetTR() + ImVec2(-2.f, 2.f + size));

	bool hovered;
	bool held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

	//Render
	const ImU32 col = window->Color((held && hovered) ? ImGuiCol_CloseButtonActive : hovered ? ImGuiCol_CloseButtonHovered : ImGuiCol_CloseButton);
	const ImVec2 center = bb.GetCenter();
	window->DrawList->AddCircleFilled(center, ImMax(2.f, size * 0.5f), col, 16);

	const float crossExtent = (size * 0.5f * 0.7071f) - 1.f;
	if (hovered)
	{
		window->DrawList->AddLine(center + ImVec2(+crossExtent, +crossExtent), center + ImVec2(-crossExtent, -crossExtent), window->Color(ImGuiCol_Text));
		window->DrawList->AddLine(center + ImVec2(+crossExtent, -crossExtent), center + ImVec2(-crossExtent, +crossExtent), window->Color(ImGuiCol_Text));
	}

	if (pOpened != NULL && pressed)
		*pOpened = !*pOpened;

	return pressed;
}

void ImGui::Image(ImTextureID userTextureID, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tintCol, const ImVec4& borderCol)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
	if (borderCol.w > 0.f)
		bb.Max += ImVec2(2, 2);
	ItemSize(bb);
	if (!ItemAdd(bb, NULL))
		return;

	if (borderCol.w > 0.f)
	{
		window->DrawList->AddRect(bb.Min, bb.Max, window->Color(borderCol), 0.f);
		window->DrawList->AddImage(userTextureID, bb.Min + ImVec2(1, 1), bb.Max - ImVec2(1, 1), uv0, uv1, window->Color(tintCol));
	}
	else
		window->DrawList->AddImage(userTextureID, bb.Min, bb.Max, uv0, uv1, window->Color(tintCol));
}

//framePadding < 0: uses FramePadding from style (default)
//framePadding = 0: no framing
//framePadding > 0: set framing size
//The color used are the button colors.
bool ImGui::ImageButton(ImTextureID userTextureID, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int framePadding, const ImVec4& bgCol, const ImVec4& tintCol)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	//Default to using texture ID as ID. User can still push string/integer prefixes.
	//We could hash the size/uv to create a unique ID but that would prevent the user from animating UV.
	ImGui::PushID((void*)userTextureID);
	const ImGuiID id = window->GetID("#image");
	ImGui::PopID();

	const ImVec2 padding = (framePadding >= 0) ? ImVec2((float)framePadding, (float)framePadding) : style.FramePadding;
	const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size + padding * 2);
	const ImRect imageBB(window->DC.CursorPos + padding, window->DC.CursorPos + padding + size);
	ItemSize(bb);
	if (!ItemAdd(bb, &id))
		return false;

	bool hovered;
	bool held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held);

	//Render
	const ImU32 col = window->Color((hovered && held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.f, style.FrameRounding));
	if (bgCol.w > 0.f)
		window->DrawList->AddRectFilled(imageBB.Min, imageBB.Max, window->Color(bgCol));
	window->DrawList->AddImage(userTextureID, imageBB.Min, imageBB.Max, uv0, uv1, window->Color(tintCol));

	return pressed;
}

//Up to here
//Start logging ImGui output to TTY
void ImGui::LogToTTY(int maxDepth)
{
	ImGuiState& g = *GImGui;
	if (g.LogEnabled)
		return;
	ImGuiWindow* window = GetCurrentWindowRead();

	g.LogEnabled = true;
	g.LogFile = stdout;
	g.LogStartDepth = window->DC.TreeDepth;
	if (maxDepth >= 0)
		g.LogAutoExpandMaxDepth = maxDepth;
}

//Start logging ImGui output to given file
void ImGui::LogToFile(int maxDepth, const char* filename)
{
	ImGuiState& g = *GImGui;
	if (g.LogEnabled)
		return;
	ImGuiWindow* window = GetCurrentWindowRead();

	if (!filename)
	{
		filename = g.IO.LogFilename;
		if (!filename)
			return;
	}

	g.LogFile = fopen(filename, "ab");
	if (!g.LogFile)
	{
		IM_ASSERT(g.LogFile != NULL);	//Consider this an error
		return;
	}
	g.LogEnabled = true;
	g.LogStartDepth = window->DC.TreeDepth;
	if (maxDepth >= 0)
		g.LogAutoExpandMaxDepth = maxDepth;
}

//Start logging ImGui output to clipboard
void ImGui::LogToClipboard(int maxDepth)
{
	ImGuiState& g = *GImGui;
	if (g.LogEnabled)
		return;
	ImGuiWindow* window = GetCurrentWindowRead();

	g.LogEnabled = true;
	g.LogFile = NULL;
	g.LogStartDepth = window->DC.TreeDepth;
	if (maxDepth >= 0)
		g.LogAutoExpandMaxDepth = maxDepth;
}

void ImGui::LogFinish()
{
	ImGuiState& g = *GImGui;
	if (!g.LogEnabled)
		return;

	ImGui::LogText(IM_NEWLINE);
	g.LogEnabled = false;
	if (g.LogFile != NULL)
	{
		if (g.LogFile == stdout)
			fflush(g.LogFile);
		else
			fclose(g.LogFile);
		g.LogFile = NULL;
	}
	if (g.LogClipboard->size() > 1)
	{
		if (g.IO.SetClipboardTextFn)
			g.IO.SetClipboardTextFn(g.LogClipboard->begin());
		g.LogClipboard->clear();
	}
}

//Helper to display logging buttons
void ImGui::LogButtons()
{
	ImGuiState& g = *GImGui;

	ImGui::PushID("LogButtons");
	const bool logToTTY = ImGui::Button("Log To TTY");
	ImGui::SameLine();
	const bool logToFile = ImGui::Button("Log To File");
	ImGui::SameLine();
	const bool logToClipboard = ImGui::Button("Log To Clipboard");
	ImGui::SameLine();

	ImGui::PushItemWidth(80.f);
	ImGui::PushAllowKeyboardFocus(false);
	ImGui::SliderInt("Depth", &g.LogAutoExpandMaxDepth, 0, 9, NULL);
	ImGui::PopAllowKeyboardFocus();
	ImGui::PopItemWidth();
	ImGui::PopID();

	//Start logging at the end of the function so that the buttons don't appear in the log
	if (logToTTY)
		LogToTTY(g.LogAutoExpandMaxDepth);
	if (logToFile)
		LogToFile(g.LogAutoExpandMaxDepth, g.IO.LogFilename);
	if (logToClipboard)
		LogToClipboard(g.LogAutoExpandMaxDepth);
}

bool ImGui::TreeNodeBehaviorIsOpened(ImGuiID id, ImGuiTreeNodeFlags flags)
{
	//We only write to the tree storage if the user clicks (or explicitely use SetNextTreeNode*** functions)
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImGuiStorage* storage = window->DC.StateStorage;

	bool opened;
	if (g.SetNextTreeNodeOpenedCond != 0)
	{
		if (g.SetNextTreeNodeOpenedCond & ImGuiSetCond_Always)
		{
			opened = g.SetNextTreeNodeOpenedVal;
			storage->SetInt(id, opened);
		}
		else
		{
			//We treat ImGuiSetCondition_Once and ImGuiSetCondition_FirstUseEver the same because tree node state are not saved persistently.
			const int storedValue = storage->GetInt(id, -1);
			if (storedValue == -1)
			{
				opened = g.SetNextTreeNodeOpenedVal;
				storage->SetInt(id, opened);
			}
			else
				opened = storedValue != 0;
		}
		g.SetNextTreeNodeOpenedCond = 0;
	}
	else
		opened = storage->GetInt(id, (flags & ImGuiTreeNodeFlags_DefaultOpen) ? 1 : 0) != 0;

	//When logging is enabled, we automatically expand tree nodes (but *NOT* collapsing headers.. seems like sensible behavior).
	//NB- If we are above max depth we still allow manually opened nodes to be logged.
	if (g.LogEnabled && !(flags & ImGuiTreeNodeFlags_NoAutoExpandOnLog) && window->DC.TreeDepth < g.LogAutoExpandMaxDepth)
		opened = true;

	return opened;
}

bool ImGui::CollapsingHeader(const char* label, const char* strID, bool displayFrame, bool defaultOpen)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	IM_ASSERT(strID != NULL || label != NULL);
	if (strID == NULL)
		strID = label;
	if (label == NULL)
		label = strID;
	const ImGuiID id = window->GetID(strID);

	//Framed header expand a little outside the default padding
	const ImVec2 windowPadding = window->WindowPadding;
	const ImVec2 labelSize = CalcTextSize(label, NULL, true);
	const ImVec2 posMin = window->DC.CursorPos;
	const ImVec2 posMax = window->Pos + GetContentRegionMax();
	ImRect bb = ImRect(posMin, ImVec2(posMax.x, posMin.y + labelSize.y));
	if (displayFrame)
	{
		bb.Min.x -= (float)(int)(windowPadding.x * 0.5f) - 1;
		bb.Max.x += (float)(int)(windowPadding.x * 0.5f) - 1;
		bb.Max.y += style.FramePadding.y * 2;
	}

	const float collapserWidth = g.FontSize + style.FramePadding.x * 2;
	const ImRect textBB(bb.Min, bb.Min + ImVec2(collapserWidth + style.FramePadding.x * 2 * 0 + (labelSize.x > 0.f ? labelSize.x : 0.f), labelSize.y));
	ItemSize(ImVec2(textBB.GetSize().x, bb.GetSize().y), displayFrame ? style.FramePadding.y : 0.f);

	const ImRect interactBB = displayFrame ? bb : ImRect(textBB.Min, textBB.Max + ImVec2(style.FramePadding.x * 2, 0.f));	//FIXME
	bool opened = TreeNodeBehaviorIsOpened(id, (defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen : 0) | (displayFrame ? ImGuiTreeNodeFlags_NoAutoExpandOnLog : 0));

	if (!ItemAdd(interactBB, &id))
		return opened;

	bool hovered;
	bool held;
	bool pressed = ButtonBehavior(interactBB, id, &hovered, &held, ImGuiButtonFlags_NoKeyModifiers);
	if (pressed)
	{
		opened = !opened;
		window->DC.StateStorage->SetInt(id, opened);
	}

	//Render
	const ImU32 col = window->Color((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
	if (displayFrame)
	{
		//Framed type
		RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
		RenderCollapseTriangle(bb.Min + style.FramePadding, opened, 1.f, true);
		if (g.LogEnabled)
		{
			//NB: '##' is normally used to hide text (as a library-wide feature), so we need to specify the text range to make sure the ## aren't stripped out here.
			const char logPrefix[] = "\n##";
			LogRenderedText(bb.Min + style.FramePadding, logPrefix, logPrefix + 3);
		}
		RenderTextClipped(bb.Min + style.FramePadding + ImVec2(collapserWidth, 0), bb.Max, label, NULL, &labelSize);
		if (g.LogEnabled)
		{
			const char logSuffix[] = "##";
			LogRenderedText(bb.Min + style.FramePadding, logSuffix, logSuffix + 2);
		}
	}
	else
	{
		//Unframed typed for tree nodes
		if (hovered)
			RenderFrame(bb.Min, bb.Max, col, false);
		RenderCollapseTriangle(bb.Min + ImVec2(style.FramePadding.x, g.FontSize * 0.15f), opened, 0.7f, false);
		if (g.LogEnabled)
			LogRenderedText(bb.Min, ">");
		RenderText(bb.Min + ImVec2(collapserWidth, 0), label);
	}

	return opened;
}

void ImGui::Bullet()
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const float lineHeight = g.FontSize;
	const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(lineHeight, lineHeight));
	ItemSize(bb);
	if (!ItemAdd(bb, NULL))
	{
		ImGui::SameLine(0, -1);
		return;
	}

	//Render
	const float bulletSize = lineHeight * 0.15f;
	window->DrawList->AddCircleFilled(bb.Min + ImVec2(style.FramePadding.x + lineHeight * 0.5f, lineHeight * 0.5f), bulletSize, window->Color(ImGuiCol_Text));

	//Stay on same line
	ImGui::SameLine(0, -1);
}

//Text with a little bullet aligned to the typical tree node.
void ImGui::BulletTextV(const char* fmt, va_list args)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	const char* textBegin = g.TempBuffer;
	const char* textEnd = textBegin + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
	const float lineHeight = g.FontSize;
	const ImVec2 labelSize = CalcTextSize(textBegin, textEnd, true);
	const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(lineHeight + (labelSize.x > 0.f ? (style.FramePadding.x * 2) : 0.f), 0) + labelSize);	//Empty text doesn't add padding
	ItemSize(bb);
	if (!ItemAdd(bb, NULL))
		return;

	//Render
	const float bulletSize = lineHeight * 0.15f;
	window->DrawList->AddCircleFilled(bb.Min + ImVec2(style.FramePadding.x + lineHeight * 0.5f, lineHeight * 0.5f), bulletSize, window->Color(ImGuiCol_Text));
	RenderText(bb.Min + ImVec2(g.FontSize + style.FramePadding.x * 2, 0), textBegin, textEnd);
}

void ImGui::BulletText(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	BulletTextV(fmt, args);
	va_end(args);
}

//If returning 'true' the node is open and the user is responsible for calling TreePop
bool ImGui::TreeNodeV(const char* strID, const char* fmt, va_list args)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
	if (!strID || !strID[0])
		strID = fmt;

	ImGui::PushID(strID);
	const bool opened = ImGui::CollapsingHeader(g.TempBuffer, "", false);
	ImGui::PopID();

	if (opened)
		ImGui::TreePush(strID);

	return opened;
}

bool ImGui::TreeNode(const char* strID, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	bool s = TreeNodeV(strID, fmt, args);
	va_end(args);
	return s;
}

//If returning 'true' the node is open and the user is responsible for calling TreePop
bool ImGui::TreeNodeV(const void* ptrID, const char* fmt, va_list args)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);

	if (!ptrID)
		ptrID = fmt;

	ImGui::PushID(ptrID);
	const bool opened = ImGui::CollapsingHeader(g.TempBuffer, "", false);
	ImGui::PopID();

	if (opened)
		ImGui::TreePush(ptrID);

	return opened;
}

bool ImGui::TreeNode(const void* ptrID, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	bool s = TreeNodeV(ptrID, fmt, args);
	va_end(args);
	return s;
}

bool ImGui::TreeNode(const char* strLabelID)
{
	return TreeNode(strLabelID, "%s", strLabelID);
}

void ImGui::SetNextTreeNodeOpened(bool opened, ImGuiSetCond cond)
{
	ImGuiState& g = *GImGui;
	g.SetNextTreeNodeOpenedVal = opened;
	g.SetNextTreeNodeOpenedCond = cond ? cond : ImGuiSetCond_Always;
}

void ImGui::PushID(const char* strID)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->IDStack.push_back(window->GetID(strID));
}

void ImGui::PushID(const char* strIDBegin, const char* strIDEnd)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->IDStack.push_back(window->GetID(strIDBegin, strIDEnd));
}

void ImGui::PushID(const void* ptrID)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->IDStack.push_back(window->GetID(ptrID));
}

void ImGui::PushID(int intID)
{
	const void* ptrID = (void*)(intptr_t)intID;
	ImGuiWindow* window = GetCurrentWindow();
	window->IDStack.push_back(window->GetID(ptrID));
}

void ImGui::PopID()
{
	ImGuiWindow* window = GetCurrentWindow();
	window->IDStack.pop_back();
}

ImGuiID ImGui::GetID(const char* strID) { return GImGui->CurrentWindow->GetID(strID); }

ImGuiID ImGui::GetID(const char* strIDBegin, const char* strIDEnd) { return GImGui->CurrentWindow->GetID(strIDBegin, strIDEnd); }

ImGuiID ImGui::GetID(const void* ptrID) { return GImGui->CurrentWindow->GetID(ptrID); }

static inline void DataTypeFormatString(ImGuiDataType dataType, void* dataPtr, const char* displayFormat, char* buf, int bufSize)
{
	if (dataType == ImGuiDataType_Int)
		ImFormatString(buf, bufSize, displayFormat, *(int*)dataPtr);
	else if (dataType == ImGuiDataType_Float)
		ImFormatString(buf, bufSize, displayFormat, *(float*)dataPtr);
}

static inline void DataTypeFormatString(ImGuiDataType dataType, void* dataPtr, int decimalPrecision, char* buf, int bufSize)
{
	if (dataType == ImGuiDataType_Int)
	{
		if (decimalPrecision < 0)
			ImFormatString(buf, bufSize, "%d", *(int*)dataPtr);
		else
			ImFormatString(buf, bufSize, "%.*d", decimalPrecision, *(int*)dataPtr);
	}
	else if (dataType == ImGuiDataType_Float)
	{
		if (decimalPrecision < 0)
			ImFormatString(buf, bufSize, "%f", *(float*)dataPtr);	//Ideally we'd have a minimum decimal precision of 1 to visually denote that it is a float, while hiding non-significant digits?
		else
			ImFormatString(buf, bufSize, "%.*f", decimalPrecision, *(float*)dataPtr);
	}
}

static void DataTypeApplyOp(ImGuiDataType dataType, int op, void* value1, const void* value2)	//Store into value1
{
	if (dataType == ImGuiDataType_Int)
	{
		if (op == '+')
			*(int*)value1 = *(int*)value1 + *(const int*)value2;
		else if (op == '-')
			*(int*)value1 = *(int*)value1 - *(const int*)value2;
	}
	else if (dataType == ImGuiDataType_Float)
	{
		if (op == '+')
			*(float*)value1 = *(float*)value1 + *(const float*)value2;
		else if (op == '-')
			*(float*)value1 = *(float*)value1 - *(const float*)value2;
	}
}

//User can input math operators (e.g. +100) to edit a numerical values.
static void DataTypeApplyOpFromText(const char* buf, const char* initialValueBuf, ImGuiDataType dataType, void* dataPtr, const char* scalarFormat)
{
	while (ImCharIsSpace(*buf))
		buf++;

	//We don't support '-' op because it would conflict with inputing negative value.
	//Instead you can use +-100 to subtract from an existing value
	char op = buf[0];
	if (op == '+' || op == '*' || op == '/')
	{
		buf++;
		while (ImCharIsSpace(*buf))
			buf++;
	}
	else
		op = 0;
	if (!buf[0])
		return;

	if (dataType == ImGuiDataType_Int)
	{
		if (!scalarFormat)
			scalarFormat = "%d";
		int* v = (int*)dataPtr;
		int refV = *v;
		if (op && sscanf(initialValueBuf, scalarFormat, &refV) < 1)
			return;

		//Store operand in a float so we can use fractional value for multipliers (*1.1), but constant always parsed as integer so we can fit big integers (e.g. 2000000003) past float precision
		float opV = 0.f;
		if (op == '+')
		{
			if (sscanf(buf, "%f", &opV) == 1)
				*v = (int)(refV + opV);
		}	//Add (use "+-" to subtract)
		else if (op == '*')
		{
			if (sscanf(buf, "%f", &opV) == 1)
				*v = (int)(refV * opV);
		}	//Multiply
		else if (op == '/')
		{
			if (sscanf(buf, "%f", &opV) == 1 && opV != 0.f)
				*v = (int)(refV / opV);
		}	//Divide
		else
		{
			if (sscanf(buf, scalarFormat, &refV) == 1)
				*v = refV;
		}	//Assign constant
	}
	else if (dataType == ImGuiDataType_Float)
	{
		//For floats we have to ignore format with precision (e.g. "%.2f") because sscanf doesn't take them in
		scalarFormat = "%f";
		float* v = (float*)dataPtr;
		float refV = *v;
		if (op && sscanf(initialValueBuf, scalarFormat, &refV) < 1)
			return;
		float opV = 0.f;
		if (sscanf(buf, scalarFormat, &opV) < 1)
			return;

		if (op == '+')
			*v = refV + opV;	//Add (use "+-" to subtract)
		else if (op == '*')
			*v = refV * opV;	//Multiply
		else if (op == '/')
		{
			if (opV != 0.f)
				*v = refV / opV;
		}	//Divide
		else
			*v = opV;	//Assign constant
	}
}

//Create text input in place of a slider (when CTRL+Clicking on slider)
bool ImGui::InputScalarAsWidgetReplacement(const ImRect& aabb, const char* label, ImGuiDataType dataType, void* dataPtr, ImGuiID id, int decimalPrecision)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();

	//Our replacement widget will override the focus ID (registered previously to allow for a TAB focus to happen)
	SetActiveID(g.ScalarAsInputTextId, window);
	SetHoveredID(0);
	FocusableItemUnregister(window);

	char buf[32];
	DataTypeFormatString(dataType, dataPtr, decimalPrecision, buf, IM_ARRAYSIZE(buf));
	bool valueChanged = InputTextEx(label, buf, IM_ARRAYSIZE(buf), aabb.GetSize() - g.Style.FramePadding * 2.f, ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_AutoSelectAll);
	if (g.ScalarAsInputTextId == 0)
	{
		//First frame
		IM_ASSERT(g.ActiveId == id);	//InputText ID expected to match the Slider ID (else we'd need to store them both, which is also possible)
		g.ScalarAsInputTextId = g.ActiveId;
		SetHoveredID(id);
	}
	else if (g.ActiveId != g.ScalarAsInputTextId)	//Release
		g.ScalarAsInputTextId = 0;
	if (valueChanged)
		DataTypeApplyOpFromText(buf, GImGui->InputTextState.InitialText.begin(), dataType, dataPtr, NULL);
	return valueChanged;
}

//Parse display precision back from the display format string
int ImGui::ParseFormatPrecision(const char* fmt, int defaultPrecision)
{
	int precision = defaultPrecision;
	while ((fmt = strchr(fmt, '%')) != NULL)
	{
		fmt++;
		if (fmt[0] == '%')
		{
			fmt++;
			continue;
		}	//Ignore "%%"
		while (*fmt >= '0' && *fmt <= '9')
			fmt++;
		if (*fmt == '.')
		{
			precision = atoi(fmt + 1);
			if (precision < 0 || precision > 10)
				precision = defaultPrecision;
		}
		break;
	}
	return precision;
}

float ImGui::RoundScalar(float value, int decimalPrecision)
{
	//Round past decimal precision
	//So when our value is 1.99999 with a precision of 0.001 we'll end up rounding to 2.0
	//FIXME: Investigate better rounding methods
	static const float minSteps[10] = { 1.f, 0.1f, 0.01f, 0.001f, 0.0001f, 0.00001f, 0.000001f, 0.0000001f, 0.00000001f, 0.000000001f };
	float minStep = (decimalPrecision >= 0 && decimalPrecision < 10) ? minSteps[decimalPrecision] : powf(10.f, (float)-decimalPrecision);
	bool negative = value < 0.f;
	value = fabsf(value);
	float remainder = fmodf(value, minStep);
	if (remainder <= minStep * 0.5f)
		value -= remainder;
	else
		value += (minStep - remainder);
	return negative ? -value : value;
}

bool ImGui::SliderBehavior(const ImRect& frameBB, ImGuiID id, float* v, float vMin, float vMax, float power, int decimalPrecision, ImGuiSliderFlags flags)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();
	const ImGuiStyle& style = g.Style;

	//Draw frame
	RenderFrame(frameBB.Min, frameBB.Max, window->Color(ImGuiCol_FrameBg), true, style.FrameRounding);

	const bool isNonLinear = fabsf(power - 1.f) > 0.0001f;
	const bool isHorizontal = (flags & ImGuiSliderFlags_Vertical) == 0;

	const float grabPadding = 2.f;
	const float sliderSZ = isHorizontal ? (frameBB.GetWidth() - grabPadding * 2.f) : (frameBB.GetHeight() - grabPadding * 2.f);
	float grabSZ;
	if (decimalPrecision > 0)
		grabSZ = ImMin(style.GrabMinSize, sliderSZ);
	else
		grabSZ = ImMin(ImMax(1.f * (sliderSZ / (vMax - vMin + 1.f)), style.GrabMinSize), sliderSZ);	//Integer sliders, if possible have the grab size represent 1 unit
	const float sliderUsableSZ = sliderSZ - grabSZ;
	const float sliderUsablePosMin = (isHorizontal ? frameBB.Min.x : frameBB.Min.y) + grabPadding + grabSZ * 0.5f;
	const float sliderUsablePosMax = (isHorizontal ? frameBB.Max.x : frameBB.Max.y) - grabPadding - grabSZ * 0.5f;

	//For logarithmic sliders that cross over sign boundary we want the exponential increase to be symmetric around 0.f
	float linearZeroPos = 0.f;	//0.0->1.f
	if (vMin * vMax < 0.f)
	{
		//Different sign
		const float linearDistMinTo0 = powf(fabsf(0.f - vMin), 1.f / power);
		const float linearDistMaxTo0 = powf(fabsf(vMax - 0.f), 1.f / power);
		linearZeroPos = linearDistMinTo0 / (linearDistMinTo0 + linearDistMaxTo0);
	}
	else	//Same sign
		linearZeroPos = vMin < 0.f ? 1.f : 0.f;

	//Process clicking on the slider
	bool valueChanged = false;
	if (g.ActiveId == id)
	{
		if (g.IO.MouseDown[0])
		{
			const float mouseAbsPos = isHorizontal ? g.IO.MousePos.x : g.IO.MousePos.y;
			float normalizedPos = ImClamp((mouseAbsPos - sliderUsablePosMin) / sliderUsableSZ, 0.f, 1.f);
			if (!isHorizontal)
				normalizedPos = 1.f - normalizedPos;

			float newValue;
			if (isNonLinear)
			{
				//Account for logarithmic scale on both sides of the zero
				if (normalizedPos < linearZeroPos)
				{
					//Negative: rescale to the negative range before powering
					float a = 1.f - (normalizedPos / linearZeroPos);
					a = powf(a, power);
					newValue = ImLerp(ImMin(vMax, 0.f), vMin, a);
				}
				else
				{
					//Positive: rescale to the positive range before powering
					float a;
					if (fabsf(linearZeroPos - 1.f) > 1.e-6)
						a = (normalizedPos - linearZeroPos) / (1.f - linearZeroPos);
					else
						a = normalizedPos;
					a = powf(a, power);
					newValue = ImLerp(ImMax(vMin, 0.f), vMax, a);
				}
			}
			else
			{
				//Linear slider
				newValue = ImLerp(vMin, vMax, normalizedPos);
			}

			//Round past decimal precision
			newValue = RoundScalar(newValue, decimalPrecision);
			if (*v != newValue)
			{
				*v = newValue;
				valueChanged = true;
			}
		}
		else
			SetActiveID(0);
	}

	//Calculate slider grab positioning
	float grabT;
	if (isNonLinear)
	{
		float vClamped = ImClamp(*v, vMin, vMax);
		if (vClamped < 0.f)
		{
			const float f = 1.f - (vClamped - vMin) / (ImMin(0.f, vMax) - vMin);
			grabT = (1.f - powf(f, 1.f / power)) * linearZeroPos;
		}
		else
		{
			const float f = (vClamped - ImMax(0.f, vMin)) / (vMax - ImMax(0.f, vMin));
			grabT = linearZeroPos + powf(f, 1.f / power) * (1.f - linearZeroPos);
		}
	}
	else
	{
		//Linear slider
		grabT = (ImClamp(*v, vMin, vMax) - vMin) / (vMax - vMin);
	}

	//Draw
	if (!isHorizontal)
		grabT = 1.f - grabT;
	const float grabPos = ImLerp(sliderUsablePosMin, sliderUsablePosMax, grabT);
	ImRect grabBB;
	if (isHorizontal)
		grabBB = ImRect(ImVec2(grabPos - grabSZ * 0.5f, frameBB.Min.y + grabPadding), ImVec2(grabPos + grabSZ * 0.5f, frameBB.Max.y - grabPadding));
	else
		grabBB = ImRect(ImVec2(frameBB.Min.x + grabPadding, grabPos - grabSZ * 0.5f), ImVec2(frameBB.Max.x - grabPadding, grabPos + grabSZ * 0.5f));
	window->DrawList->AddRectFilled(grabBB.Min, grabBB.Max, window->Color(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

	return valueChanged;
}

//Up to here
//Use power!=1.0 for logarithmic sliders.
//Adjust displayFormat to decorate the value with a prefix or a suffix.
//  "%.3f"         1.234
//  "%5.2f secs"   01.23 secs
//  "Gold: %.f"   Gold: 1
bool ImGui::SliderFloat(const char* label, float* v, float vMin, float vMax, const char* displayFormat, float power)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const float w = CalcItemWidth();

	const ImVec2 labelSize = CalcTextSize(label, NULL, true);
	const ImRect frameBB(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, labelSize.y) + style.FramePadding * 2.f);
	const ImRect totalBB(frameBB.Min, frameBB.Max + ImVec2(labelSize.x > 0.f ? style.ItemInnerSpacing.x + labelSize.x : 0.f, 0.f));

	//NB- we don't call ItemSize() yet because we may turn into a text edit box below
	if (!ItemAdd(totalBB, &id))
	{
		ItemSize(totalBB, style.FramePadding.y);
		return false;
	}

	const bool hovered = IsHovered(frameBB, id);
	if (hovered)
		SetHoveredID(id);

	if (!displayFormat)
		displayFormat = "%.3f";
	int decimalPrecision = ParseFormatPrecision(displayFormat, 3);

	//Tabbing or CTRL-clicking on Slider turns it into an input box
	bool start_text_input = false;
	const bool tab_focus_requested = FocusableItemRegister(window, g.ActiveId == id);
	if (tab_focus_requested || (hovered && g.IO.MouseClicked[0]))
	{
		SetActiveID(id, window);
		FocusWindow(window);

		if (tab_focus_requested || g.IO.KeyCtrl)
		{
			start_text_input = true;
			g.ScalarAsInputTextId = 0;
		}
	}
	if (start_text_input || (g.ActiveId == id && g.ScalarAsInputTextId == id))
		return InputScalarAsWidgetReplacement(frameBB, label, ImGuiDataType_Float, v, id, decimalPrecision);

	ItemSize(totalBB, style.FramePadding.y);

	//Actual slider behavior + render grab
	const bool valueChanged = SliderBehavior(frameBB, id, v, vMin, vMax, power, decimalPrecision);

	//Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
	char value_buf[64];
	const char* value_bufEnd = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), displayFormat, *v);
	RenderTextClipped(frameBB.Min, frameBB.Max, value_buf, value_bufEnd, NULL, ImGuiAlign_Center | ImGuiAlign_VCenter);

	if (labelSize.x > 0.f)
		RenderText(ImVec2(frameBB.Max.x + style.ItemInnerSpacing.x, frameBB.Min.y + style.FramePadding.y), label);

	return valueChanged;
}

bool ImGui::VSliderFloat(const char* label, const ImVec2& size, float* v, float vMin, float vMax, const char* displayFormat, float power)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	const ImVec2 labelSize = CalcTextSize(label, NULL, true);
	const ImRect frameBB(window->DC.CursorPos, window->DC.CursorPos + size);
	const ImRect bb(frameBB.Min, frameBB.Max + ImVec2(labelSize.x > 0.f ? style.ItemInnerSpacing.x + labelSize.x : 0.f, 0.f));

	ItemSize(bb, style.FramePadding.y);
	if (!ItemAdd(frameBB, &id))
		return false;

	const bool hovered = IsHovered(frameBB, id);
	if (hovered)
		SetHoveredID(id);

	if (!displayFormat)
		displayFormat = "%.3f";
	int decimalPrecision = ParseFormatPrecision(displayFormat, 3);

	if (hovered && g.IO.MouseClicked[0])
	{
		SetActiveID(id, window);
		FocusWindow(window);
	}

	//Actual slider behavior + render grab
	bool valueChanged = SliderBehavior(frameBB, id, v, vMin, vMax, power, decimalPrecision, ImGuiSliderFlags_Vertical);

	//Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
	//For the vertical slider we allow centered text to overlap the frame padding
	char value_buf[64];
	char* value_bufEnd = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), displayFormat, *v);
	RenderTextClipped(ImVec2(frameBB.Min.x, frameBB.Min.y + style.FramePadding.y), frameBB.Max, value_buf, value_bufEnd, NULL, ImGuiAlign_Center);

	if (labelSize.x > 0.f)
		RenderText(ImVec2(frameBB.Max.x + style.ItemInnerSpacing.x, frameBB.Min.y + style.FramePadding.y), label);

	return valueChanged;
}

bool ImGui::SliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max)
{
	float v_deg = (*v_rad) * 360.f / (2 * IM_PI);
	bool valueChanged = ImGui::SliderFloat(label, &v_deg, v_degrees_min, v_degrees_max, "%.f deg", 1.f);
	*v_rad = v_deg * (2 * IM_PI) / 360.f;
	return valueChanged;
}

bool ImGui::SliderInt(const char* label, int* v, int vMin, int vMax, const char* displayFormat)
{
	if (!displayFormat)
		displayFormat = "%.f";
	float v_f = (float)*v;
	bool valueChanged = ImGui::SliderFloat(label, &v_f, (float)vMin, (float)vMax, displayFormat, 1.f);
	*v = (int)v_f;
	return valueChanged;
}

bool ImGui::VSliderInt(const char* label, const ImVec2& size, int* v, int vMin, int vMax, const char* displayFormat)
{
	if (!displayFormat)
		displayFormat = "%.f";
	float v_f = (float)*v;
	bool valueChanged = ImGui::VSliderFloat(label, size, &v_f, (float)vMin, (float)vMax, displayFormat, 1.f);
	*v = (int)v_f;
	return valueChanged;
}

//Add multiple sliders on 1 line for compact edition of multiple components
bool ImGui::SliderFloatN(const char* label, float* v, int components, float vMin, float vMax, const char* displayFormat, float power)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	bool valueChanged = false;
	ImGui::BeginGroup();
	ImGui::PushID(label);
	PushMultiItemsWidths(components);
	for (int i = 0; i < components; i++)
	{
		ImGui::PushID(i);
		valueChanged |= ImGui::SliderFloat("##v", &v[i], vMin, vMax, displayFormat, power);
		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		ImGui::PopID();
		ImGui::PopItemWidth();
	}
	ImGui::PopID();

	ImGui::TextUnformatted(label, FindTextDisplayEnd(label));
	ImGui::EndGroup();

	return valueChanged;
}

bool ImGui::SliderFloat2(const char* label, float v[2], float vMin, float vMax, const char* displayFormat, float power)
{
	return SliderFloatN(label, v, 2, vMin, vMax, displayFormat, power);
}

bool ImGui::SliderFloat3(const char* label, float v[3], float vMin, float vMax, const char* displayFormat, float power)
{
	return SliderFloatN(label, v, 3, vMin, vMax, displayFormat, power);
}

bool ImGui::SliderFloat4(const char* label, float v[4], float vMin, float vMax, const char* displayFormat, float power)
{
	return SliderFloatN(label, v, 4, vMin, vMax, displayFormat, power);
}

bool ImGui::SliderIntN(const char* label, int* v, int components, int vMin, int vMax, const char* displayFormat)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	bool valueChanged = false;
	ImGui::BeginGroup();
	ImGui::PushID(label);
	PushMultiItemsWidths(components);
	for (int i = 0; i < components; i++)
	{
		ImGui::PushID(i);
		valueChanged |= ImGui::SliderInt("##v", &v[i], vMin, vMax, displayFormat);
		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		ImGui::PopID();
		ImGui::PopItemWidth();
	}
	ImGui::PopID();

	ImGui::TextUnformatted(label, FindTextDisplayEnd(label));
	ImGui::EndGroup();

	return valueChanged;
}

bool ImGui::SliderInt2(const char* label, int v[2], int vMin, int vMax, const char* displayFormat)
{
	return SliderIntN(label, v, 2, vMin, vMax, displayFormat);
}

bool ImGui::SliderInt3(const char* label, int v[3], int vMin, int vMax, const char* displayFormat)
{
	return SliderIntN(label, v, 3, vMin, vMax, displayFormat);
}

bool ImGui::SliderInt4(const char* label, int v[4], int vMin, int vMax, const char* displayFormat)
{
	return SliderIntN(label, v, 4, vMin, vMax, displayFormat);
}

bool ImGui::DragBehavior(const ImRect& frameBB, ImGuiID id, float* v, float v_speed, float vMin, float vMax, int decimalPrecision, float power)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();
	const ImGuiStyle& style = g.Style;

	//Draw frame
	const ImU32 frame_col = window->Color(g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
	RenderFrame(frameBB.Min, frameBB.Max, frame_col, true, style.FrameRounding);

	bool valueChanged = false;

	//Process clicking on the drag
	if (g.ActiveId == id)
	{
		if (g.IO.MouseDown[0])
		{
			if (g.ActiveIdIsJustActivated)
			{
				//Lock current value on click
				g.DragCurrentValue = *v;
				g.DragLastMouseDelta = ImVec2(0.f, 0.f);
			}

			const ImVec2 mouse_drag_delta = ImGui::GetMouseDragDelta(0, 1.f);
			if (fabsf(mouse_drag_delta.x - g.DragLastMouseDelta.x) > 0.f)
			{
				float speed = v_speed;
				if (speed == 0.f && (vMax - vMin) != 0.f && (vMax - vMin) < FLT_MAX)
					speed = (vMax - vMin) * g.DragSpeedDefaultRatio;
				if (g.IO.KeyShift && g.DragSpeedScaleFast >= 0.f)
					speed = speed * g.DragSpeedScaleFast;
				if (g.IO.KeyAlt && g.DragSpeedScaleSlow >= 0.f)
					speed = speed * g.DragSpeedScaleSlow;

				float v_cur = g.DragCurrentValue;
				float delta = (mouse_drag_delta.x - g.DragLastMouseDelta.x) * speed;
				if (fabsf(power - 1.f) > 0.001f)
				{
					//Logarithmic curve on both side of 0.0
					float v0_abs = v_cur >= 0.f ? v_cur : -v_cur;
					float v0_sign = v_cur >= 0.f ? 1.f : -1.f;
					float v1 = powf(v0_abs, 1.f / power) + (delta * v0_sign);
					float v1_abs = v1 >= 0.f ? v1 : -v1;
					float v1_sign = v1 >= 0.f ? 1.f : -1.f;	//Crossed sign line
					v_cur = powf(v1_abs, power) * v0_sign * v1_sign;	//Reapply sign
				}
				else
					v_cur += delta;
				g.DragLastMouseDelta.x = mouse_drag_delta.x;

				//Clamp
				if (vMin < vMax)
					v_cur = ImClamp(v_cur, vMin, vMax);
				g.DragCurrentValue = v_cur;

				//Round to user desired precision, then apply
				v_cur = RoundScalar(v_cur, decimalPrecision);
				if (*v != v_cur)
				{
					*v = v_cur;
					valueChanged = true;
				}
			}
		}
		else
			SetActiveID(0);
	}

	return valueChanged;
}

bool ImGui::DragFloat(const char* label, float* v, float v_speed, float vMin, float vMax, const char* displayFormat, float power)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const float w = CalcItemWidth();

	const ImVec2 labelSize = CalcTextSize(label, NULL, true);
	const ImRect frameBB(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, labelSize.y) + style.FramePadding * 2.f);
	const ImRect inner_bb(frameBB.Min + style.FramePadding, frameBB.Max - style.FramePadding);
	const ImRect totalBB(frameBB.Min, frameBB.Max + ImVec2(labelSize.x > 0.f ? style.ItemInnerSpacing.x + labelSize.x : 0.f, 0.f));

	//NB- we don't call ItemSize() yet because we may turn into a text edit box below
	if (!ItemAdd(totalBB, &id))
	{
		ItemSize(totalBB, style.FramePadding.y);
		return false;
	}

	const bool hovered = IsHovered(frameBB, id);
	if (hovered)
		SetHoveredID(id);

	if (!displayFormat)
		displayFormat = "%.3f";
	int decimalPrecision = ParseFormatPrecision(displayFormat, 3);

	//Tabbing or CTRL-clicking on Drag turns it into an input box
	bool start_text_input = false;
	const bool tab_focus_requested = FocusableItemRegister(window, g.ActiveId == id);
	if (tab_focus_requested || (hovered && (g.IO.MouseClicked[0] | g.IO.MouseDoubleClicked[0])))
	{
		SetActiveID(id, window);
		FocusWindow(window);

		if (tab_focus_requested || g.IO.KeyCtrl || g.IO.MouseDoubleClicked[0])
		{
			start_text_input = true;
			g.ScalarAsInputTextId = 0;
		}
	}
	if (start_text_input || (g.ActiveId == id && g.ScalarAsInputTextId == id))
		return InputScalarAsWidgetReplacement(frameBB, label, ImGuiDataType_Float, v, id, decimalPrecision);

	//Actual drag behavior
	ItemSize(totalBB, style.FramePadding.y);
	const bool valueChanged = DragBehavior(frameBB, id, v, v_speed, vMin, vMax, decimalPrecision, power);

	//Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
	char value_buf[64];
	const char* value_bufEnd = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), displayFormat, *v);
	RenderTextClipped(frameBB.Min, frameBB.Max, value_buf, value_bufEnd, NULL, ImGuiAlign_Center | ImGuiAlign_VCenter);

	if (labelSize.x > 0.f)
		RenderText(ImVec2(frameBB.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);

	return valueChanged;
}

bool ImGui::DragFloatN(const char* label, float* v, int components, float v_speed, float vMin, float vMax, const char* displayFormat, float power)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	bool valueChanged = false;
	ImGui::BeginGroup();
	ImGui::PushID(label);
	PushMultiItemsWidths(components);
	for (int i = 0; i < components; i++)
	{
		ImGui::PushID(i);
		valueChanged |= ImGui::DragFloat("##v", &v[i], v_speed, vMin, vMax, displayFormat, power);
		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		ImGui::PopID();
		ImGui::PopItemWidth();
	}
	ImGui::PopID();

	ImGui::TextUnformatted(label, FindTextDisplayEnd(label));
	ImGui::EndGroup();

	return valueChanged;
}

bool ImGui::DragFloat2(const char* label, float v[2], float v_speed, float vMin, float vMax, const char* displayFormat, float power)
{
	return DragFloatN(label, v, 2, v_speed, vMin, vMax, displayFormat, power);
}

bool ImGui::DragFloat3(const char* label, float v[3], float v_speed, float vMin, float vMax, const char* displayFormat, float power)
{
	return DragFloatN(label, v, 3, v_speed, vMin, vMax, displayFormat, power);
}

bool ImGui::DragFloat4(const char* label, float v[4], float v_speed, float vMin, float vMax, const char* displayFormat, float power)
{
	return DragFloatN(label, v, 4, v_speed, vMin, vMax, displayFormat, power);
}

bool ImGui::DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed, float vMin, float vMax, const char* displayFormat, const char* displayFormat_max, float power)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	ImGui::PushID(label);
	ImGui::BeginGroup();
	PushMultiItemsWidths(2);

	bool valueChanged = ImGui::DragFloat("##min", v_current_min, v_speed, (vMin >= vMax) ? -FLT_MAX : vMin, (vMin >= vMax) ? *v_current_max : ImMin(vMax, *v_current_max), displayFormat, power);
	ImGui::PopItemWidth();
	ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
	valueChanged |= ImGui::DragFloat("##max", v_current_max, v_speed, (vMin >= vMax) ? *v_current_min : ImMax(vMin, *v_current_min), (vMin >= vMax) ? FLT_MAX : vMax, 
										displayFormat_max ? displayFormat_max : displayFormat, power);
	ImGui::PopItemWidth();
	ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);

	ImGui::TextUnformatted(label, FindTextDisplayEnd(label));
	ImGui::EndGroup();
	ImGui::PopID();

	return valueChanged;
}

//NB: v_speed is float to allow adjusting the drag speed with more precision
bool ImGui::DragInt(const char* label, int* v, float v_speed, int vMin, int vMax, const char* displayFormat)
{
	if (!displayFormat)
		displayFormat = "%.f";
	float v_f = (float)*v;
	bool valueChanged = ImGui::DragFloat(label, &v_f, v_speed, (float)vMin, (float)vMax, displayFormat);
	*v = (int)v_f;
	return valueChanged;
}

bool ImGui::DragIntN(const char* label, int* v, int components, float v_speed, int vMin, int vMax, const char* displayFormat)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	bool valueChanged = false;
	ImGui::BeginGroup();
	ImGui::PushID(label);
	PushMultiItemsWidths(components);
	for (int i = 0; i < components; i++)
	{
		ImGui::PushID(i);
		valueChanged |= ImGui::DragInt("##v", &v[i], v_speed, vMin, vMax, displayFormat);
		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		ImGui::PopID();
		ImGui::PopItemWidth();
	}
	ImGui::PopID();

	ImGui::TextUnformatted(label, FindTextDisplayEnd(label));
	ImGui::EndGroup();

	return valueChanged;
}

bool ImGui::DragInt2(const char* label, int v[2], float v_speed, int vMin, int vMax, const char* displayFormat)
{
	return DragIntN(label, v, 2, v_speed, vMin, vMax, displayFormat);
}

bool ImGui::DragInt3(const char* label, int v[3], float v_speed, int vMin, int vMax, const char* displayFormat)
{
	return DragIntN(label, v, 3, v_speed, vMin, vMax, displayFormat);
}

bool ImGui::DragInt4(const char* label, int v[4], float v_speed, int vMin, int vMax, const char* displayFormat)
{
	return DragIntN(label, v, 4, v_speed, vMin, vMax, displayFormat);
}

bool ImGui::DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed, int vMin, int vMax, const char* displayFormat, const char* displayFormat_max)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	ImGui::PushID(label);
	ImGui::BeginGroup();
	PushMultiItemsWidths(2);

	bool valueChanged = ImGui::DragInt("##min", v_current_min, v_speed, (vMin >= vMax) ? IM_INT_MIN : vMin, (vMin >= vMax) ? *v_current_max : ImMin(vMax, *v_current_max), displayFormat);
	ImGui::PopItemWidth();
	ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
	valueChanged |= ImGui::DragInt("##max", v_current_max, v_speed, (vMin >= vMax) ? *v_current_min : ImMax(vMin, *v_current_min), (vMin >= vMax) ? IM_INT_MAX : vMax, displayFormat_max ? displayFormat_max : displayFormat);
	ImGui::PopItemWidth();
	ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);

	ImGui::TextUnformatted(label, FindTextDisplayEnd(label));
	ImGui::EndGroup();
	ImGui::PopID();

	return valueChanged;
}

void ImGui::PlotEx(ImGuiPlotType plot_type, const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	const ImVec2 labelSize = ImGui::CalcTextSize(label, NULL, true);
	if (graph_size.x == 0.f)
		graph_size.x = CalcItemWidth() + (style.FramePadding.x * 2);
	if (graph_size.y == 0.f)
		graph_size.y = labelSize.y + (style.FramePadding.y * 2);

	const ImRect frameBB(window->DC.CursorPos, window->DC.CursorPos + ImVec2(graph_size.x, graph_size.y));
	const ImRect inner_bb(frameBB.Min + style.FramePadding, frameBB.Max - style.FramePadding);
	const ImRect totalBB(frameBB.Min, frameBB.Max + ImVec2(labelSize.x > 0.f ? style.ItemInnerSpacing.x + labelSize.x : 0.f, 0));
	ItemSize(totalBB, style.FramePadding.y);
	if (!ItemAdd(totalBB, NULL))
		return;

	//Determine scale from values if not specified
	if (scale_min == FLT_MAX || scale_max == FLT_MAX)
	{
		float vMin = FLT_MAX;
		float vMax = -FLT_MAX;
		for (int i = 0; i < values_count; i++)
		{
			const float v = values_getter(data, i);
			vMin = ImMin(vMin, v);
			vMax = ImMax(vMax, v);
		}
		if (scale_min == FLT_MAX)
			scale_min = vMin;
		if (scale_max == FLT_MAX)
			scale_max = vMax;
	}

	RenderFrame(frameBB.Min, frameBB.Max, window->Color(ImGuiCol_FrameBg), true, style.FrameRounding);

	int res_w = ImMin((int)graph_size.x, values_count) + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0);
	int item_count = values_count + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0);

	//Tooltip on hover
	int v_hovered = -1;
	if (IsHovered(inner_bb, 0))
	{
		const float t = ImClamp((g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.f, 0.9999f);
		const int v_idx = (int)(t * item_count);
		IM_ASSERT(v_idx >= 0 && v_idx < values_count);

		const float v0 = values_getter(data, (v_idx + values_offset) % values_count);
		const float v1 = values_getter(data, (v_idx + 1 + values_offset) % values_count);
		if (plot_type == ImGuiPlotType_Lines)
			ImGui::SetTooltip("%d: %8.4g\n%d: %8.4g", v_idx, v0, v_idx + 1, v1);
		else if (plot_type == ImGuiPlotType_Histogram)
			ImGui::SetTooltip("%d: %8.4g", v_idx, v0);
		v_hovered = v_idx;
	}

	const float t_step = 1.f / (float)res_w;

	float v0 = values_getter(data, (0 + values_offset) % values_count);
	float t0 = 0.f;
	ImVec2 tp0 = ImVec2(t0, 1.f - ImSaturate((v0 - scale_min) / (scale_max - scale_min)));	//Point in the normalized space of our target rectangle

	const ImU32 col_base = window->Color((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram);
	const ImU32 col_hovered = window->Color((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLinesHovered : ImGuiCol_PlotHistogramHovered);

	for (int n = 0; n < res_w; n++)
	{
		const float t1 = t0 + t_step;
		const int v1_idx = (int)(t0 * item_count + 0.5f);
		IM_ASSERT(v1_idx >= 0 && v1_idx < values_count);
		const float v1 = values_getter(data, (v1_idx + values_offset + 1) % values_count);
		const ImVec2 tp1 = ImVec2(t1, 1.f - ImSaturate((v1 - scale_min) / (scale_max - scale_min)));

		//NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
		ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
		ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == ImGuiPlotType_Lines) ? tp1 : ImVec2(tp1.x, 1.f));
		if (plot_type == ImGuiPlotType_Lines)
			window->DrawList->AddLine(pos0, pos1, v_hovered == v1_idx ? col_hovered : col_base);
		else if (plot_type == ImGuiPlotType_Histogram)
		{
			if (pos1.x >= pos0.x + 2.f)
				pos1.x -= 1.f;
			window->DrawList->AddRectFilled(pos0, pos1, v_hovered == v1_idx ? col_hovered : col_base);
		}

		t0 = t1;
		tp0 = tp1;
	}

	//Text overlay
	if (overlay_text)
		RenderTextClipped(ImVec2(frameBB.Min.x, frameBB.Min.y + style.FramePadding.y), frameBB.Max, overlay_text, NULL, NULL, ImGuiAlign_Center);

	RenderText(ImVec2(frameBB.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
}

struct ImGuiPlotArrayGetterData
{
    const float* Values;
    int Stride;

    ImGuiPlotArrayGetterData(const float* values, int stride)
	{
		Values = values;
		Stride = stride;
	}
};

static float Plot_ArrayGetter(void* data, int idx)
{
	ImGuiPlotArrayGetterData* plot_data = (ImGuiPlotArrayGetterData*)data;
	const float v = *(float*)(void*)((unsigned char*)plot_data->Values + (size_t)idx * plot_data->Stride);
	return v;
}

void ImGui::PlotLines(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
{
	ImGuiPlotArrayGetterData data(values, stride);
	PlotEx(ImGuiPlotType_Lines, label, &Plot_ArrayGetter, (void*)&data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void ImGui::PlotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
	PlotEx(ImGuiPlotType_Lines, label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void ImGui::PlotHistogram(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
{
	ImGuiPlotArrayGetterData data(values, stride);
	PlotEx(ImGuiPlotType_Histogram, label, &Plot_ArrayGetter, (void*)&data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void ImGui::PlotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
	PlotEx(ImGuiPlotType_Histogram, label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

bool ImGui::Checkbox(const char* label, bool* v)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 labelSize = CalcTextSize(label, NULL, true);

	const ImRect check_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(labelSize.y + style.FramePadding.y * 2, labelSize.y + style.FramePadding.y * 2));
	ItemSize(check_bb, style.FramePadding.y);

	ImRect totalBB = check_bb;
	if (labelSize.x > 0)
		SameLine(0, style.ItemInnerSpacing.x);
	const ImRect textBB(window->DC.CursorPos + ImVec2(0, style.FramePadding.y), window->DC.CursorPos + ImVec2(0, style.FramePadding.y) + labelSize);
	if (labelSize.x > 0)
	{
		ItemSize(ImVec2(textBB.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
		totalBB = ImRect(ImMin(check_bb.Min, textBB.Min), ImMax(check_bb.Max, textBB.Max));
	}

	if (!ItemAdd(totalBB, &id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(totalBB, id, &hovered, &held);
	if (pressed)
		*v = !(*v);

	RenderFrame(check_bb.Min, check_bb.Max, window->Color((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
	if (*v)
	{
		const float check_sz = ImMin(check_bb.GetWidth(), check_bb.GetHeight());
		const float pad = ImMax(1.f, (float)(int)(check_sz / 6.f));
		window->DrawList->AddRectFilled(check_bb.Min + ImVec2(pad, pad), check_bb.Max - ImVec2(pad, pad), window->Color(ImGuiCol_CheckMark), style.FrameRounding);
	}

	if (g.LogEnabled)
		LogRenderedText(textBB.GetTL(), *v ? "[x]" : "[ ]");
	RenderText(textBB.GetTL(), label);

	return pressed;
}

bool ImGui::CheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value)
{
	bool v = (*flags & flags_value) ? true : false;
	bool pressed = ImGui::Checkbox(label, &v);
	if (v)
		*flags |= flags_value;
	else
		*flags &= ~flags_value;
	return pressed;
}

bool ImGui::RadioButton(const char* label, bool active)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 labelSize = CalcTextSize(label, NULL, true);

	const ImRect check_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(labelSize.y + style.FramePadding.y * 2 - 1, labelSize.y + style.FramePadding.y * 2 - 1));
	ItemSize(check_bb, style.FramePadding.y);

	ImRect totalBB = check_bb;
	if (labelSize.x > 0)
		SameLine(0, style.ItemInnerSpacing.x);
	const ImRect textBB(window->DC.CursorPos + ImVec2(0, style.FramePadding.y), window->DC.CursorPos + ImVec2(0, style.FramePadding.y) + labelSize);
	if (labelSize.x > 0)
	{
		ItemSize(ImVec2(textBB.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
		totalBB.Add(textBB);
	}

	if (!ItemAdd(totalBB, &id))
		return false;

	ImVec2 center = check_bb.GetCenter();
	center.x = (float)(int)center.x + 0.5f;
	center.y = (float)(int)center.y + 0.5f;
	const float radius = check_bb.GetHeight() * 0.5f;

	bool hovered, held;
	bool pressed = ButtonBehavior(totalBB, id, &hovered, &held);

	window->DrawList->AddCircleFilled(center, radius, window->Color((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), 16);
	if (active)
	{
		const float check_sz = ImMin(check_bb.GetWidth(), check_bb.GetHeight());
		const float pad = ImMax(1.f, (float)(int)(check_sz / 6.f));
		window->DrawList->AddCircleFilled(center, radius - pad, window->Color(ImGuiCol_CheckMark), 16);
	}

	if (window->Flags & ImGuiWindowFlags_ShowBorders)
	{
		window->DrawList->AddCircle(center + ImVec2(1, 1), radius, window->Color(ImGuiCol_BorderShadow), 16);
		window->DrawList->AddCircle(center, radius, window->Color(ImGuiCol_Border), 16);
	}

	if (g.LogEnabled)
		LogRenderedText(textBB.GetTL(), active ? "(x)" : "( )");
	RenderText(textBB.GetTL(), label);

	return pressed;
}

bool ImGui::RadioButton(const char* label, int* v, int v_button)
{
	const bool pressed = ImGui::RadioButton(label, *v == v_button);
	if (pressed)
		*v = v_button;
	return pressed;
}

static int InputTextCalcTextLenAndLineCount(const char* textBegin, const char** outTextEnd)
{
	int line_count = 0;
	const char* s = textBegin;
	while (char c = *s++)	//We are only matching for \n so we can ignore UTF-8 decoding
	{
		if (c == '\n')
			line_count++;
	}
	s--;
	if (s[0] != '\n' && s[0] != '\r')
		line_count++;
	*outTextEnd = s;
	return line_count;
}

static ImVec2 InputTextCalcTextSizeW(const ImWchar* textBegin, const ImWchar* textEnd, const ImWchar** remaining, ImVec2* outOffset, bool stopOnNewLine)
{
	ImFont* font = GImGui->Font;
	const float lineHeight = GImGui->FontSize;
	const float scale = lineHeight / font->FontSize;

	ImVec2 textSize = ImVec2(0, 0);
	float line_width = 0.f;

	const ImWchar* s = textBegin;
	while (s < textEnd)
	{
		unsigned int c = (unsigned int)(*s++);
		if (c == '\n')
		{
			textSize.x = ImMax(textSize.x, line_width);
			textSize.y += lineHeight;
			line_width = 0.f;
			if (stopOnNewLine)
				break;
			continue;
		}
		if (c == '\r')
			continue;

		const float char_width = font->GetCharAdvance((unsigned short)c) * scale;
		line_width += char_width;
	}

	if (textSize.x < line_width)
		textSize.x = line_width;

	if (outOffset)
		*outOffset = ImVec2(line_width, textSize.y + lineHeight);	//offset allow for the possibility of sitting after a trailing \n

	if (line_width > 0 || textSize.y == 0.f)	//whereas size.y will ignore the trailing \n
		textSize.y += lineHeight;

	if (remaining)
		*remaining = s;

	return textSize;
}

//Wrapper for stb_textedit.h to edit text (our wrapper is for: statically sized buffer, single-line, wchar characters. InputText converts between UTF-8 and wchar)
namespace ImGuiStb
{

	static int STB_TEXTEDIT_STRINGLEN(const STB_TEXTEDIT_STRING* obj) { return obj->CurLenW; }
	static ImWchar STB_TEXTEDIT_GETCHAR(const STB_TEXTEDIT_STRING* obj, int idx) { return obj->Text[idx]; }
	static float STB_TEXTEDIT_GETWIDTH(STB_TEXTEDIT_STRING* obj, int lineStart_idx, int char_idx)
	{
		ImWchar c = obj->Text[lineStart_idx + char_idx];
		if (c == '\n')
			return STB_TEXTEDIT_GETWIDTH_NEWLINE;
		return GImGui->Font->GetCharAdvance(c) * (GImGui->FontSize / GImGui->Font->FontSize);
	}
	static int STB_TEXTEDIT_KEYTOTEXT(int key) { return key >= 0x10000 ? 0 : key; }
	static ImWchar STB_TEXTEDIT_NEWLINE = '\n';
	static void STB_TEXTEDIT_LAYOUTROW(StbTexteditRow* r, STB_TEXTEDIT_STRING* obj, int lineStart_idx)
	{
		const ImWchar* text = obj->Text.Data;
		const ImWchar* textRemaining = NULL;
		const ImVec2 size = InputTextCalcTextSizeW(text + lineStart_idx, text + obj->CurLenW, &textRemaining, NULL, true);
		r->x0 = 0.f;
		r->x1 = size.x;
		r->baseline_y_delta = size.y;
		r->ymin = 0.f;
		r->ymax = size.y;
		r->num_chars = (int)(textRemaining - (text + lineStart_idx));
	}

	static bool is_separator(unsigned int c) { return c == ',' || c == ';' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == '|'; }
#define STB_TEXTEDIT_IS_SPACE(CH) (ImCharIsSpace((unsigned int)CH) || is_separator((unsigned int)CH))
	static void STB_TEXTEDIT_DELETECHARS(STB_TEXTEDIT_STRING* obj, int pos, int n)
	{
		ImWchar* dst = obj->Text.Data + pos;

		//We maintain our buffer length in both UTF-8 and wchar formats
		obj->CurLenA -= ImTextCountUtf8BytesFromStr(dst, dst + n);
		obj->CurLenW -= n;

		//Offset remaining text
		const ImWchar* src = obj->Text.Data + pos + n;
		while (ImWchar c = *src++)
			*dst++ = c;
		*dst = '\0';
	}

	static bool STB_TEXTEDIT_INSERTCHARS(STB_TEXTEDIT_STRING* obj, int pos, const ImWchar* new_text, int new_textLen)
	{
		const int textLen = obj->CurLenW;
		if (new_textLen + textLen + 1 > obj->Text.Size)
			return false;

		const int new_textLen_utf8 = ImTextCountUtf8BytesFromStr(new_text, new_text + new_textLen);
		if (new_textLen_utf8 + obj->CurLenA + 1 > obj->BufSizeA)
			return false;

		ImWchar* text = obj->Text.Data;
		if (pos != textLen)
			memmove(text + pos + new_textLen, text + pos, (size_t)(textLen - pos) * sizeof(ImWchar));
		memcpy(text + pos, new_text, (size_t)new_textLen * sizeof(ImWchar));

		obj->CurLenW += new_textLen;
		obj->CurLenA += new_textLen_utf8;
		obj->Text[obj->CurLenW] = '\0';

		return true;
	}

	//We don't use an enum so we can build even with conflicting symbols (if another user of stb_textedit.h leak their STB_TEXTEDIT_K_* symbols)
#define STB_TEXTEDIT_K_LEFT         0x10000 //keyboard input to move cursor left
#define STB_TEXTEDIT_K_RIGHT        0x10001 //keyboard input to move cursor right
#define STB_TEXTEDIT_K_UP           0x10002 //keyboard input to move cursor up
#define STB_TEXTEDIT_K_DOWN         0x10003 //keyboard input to move cursor down
#define STB_TEXTEDIT_K_LINESTART    0x10004 //keyboard input to move cursor to start of line
#define STB_TEXTEDIT_K_LINEEND      0x10005 //keyboard input to move cursor to end of line
#define STB_TEXTEDIT_K_TEXTSTART    0x10006 //keyboard input to move cursor to start of text
#define STB_TEXTEDIT_K_TEXTEND      0x10007 //keyboard input to move cursor to end of text
#define STB_TEXTEDIT_K_DELETE       0x10008 //keyboard input to delete selection or character under cursor
#define STB_TEXTEDIT_K_BACKSPACE    0x10009 //keyboard input to delete selection or character left of cursor
#define STB_TEXTEDIT_K_UNDO         0x1000A //keyboard input to perform undo
#define STB_TEXTEDIT_K_REDO         0x1000B //keyboard input to perform redo
#define STB_TEXTEDIT_K_WORDLEFT     0x1000C //keyboard input to move cursor left one word
#define STB_TEXTEDIT_K_WORDRIGHT    0x1000D //keyboard input to move cursor right one word
#define STB_TEXTEDIT_K_SHIFT        0x20000

#define STB_TEXTEDIT_IMPLEMENTATION
#include "stb_textedit.h"

}

void ImGuiTextEditState::OnKeyPressed(int key)
{
	stb_textedit_key(this, &StbState, key);
	CursorFollow = true;
	CursorAnimReset();
}

//Public API to manipulate UTF-8 text
//We expose UTF-8 to the user (unlike the STB_TEXTEDIT_* functions which are manipulating wchar)
void ImGuiTextEditCallbackData::DeleteChars(int pos, int bytesCount)
{
	char* dst = Buf + pos;
	const char* src = Buf + pos + bytesCount;
	while (char c = *src++)
		*dst++ = c;
	*dst = '\0';

	BufDirty = true;
	if (CursorPos + bytesCount >= pos)
		CursorPos -= bytesCount;
	else if (CursorPos >= pos)
		CursorPos = pos;
	SelectionStart = SelectionEnd = CursorPos;
}

void ImGuiTextEditCallbackData::InsertChars(int pos, const char* new_text, const char* new_textEnd)
{
	const int textLen = (int)strlen(Buf);
	if (!new_textEnd)
		new_textEnd = new_text + strlen(new_text);
	const int new_textLen = (int)(new_textEnd - new_text);

	if (new_textLen + textLen + 1 >= BufSize)
		return;

	if (textLen != pos)
		memmove(Buf + pos + new_textLen, Buf + pos, (size_t)(textLen - pos));
	memcpy(Buf + pos, new_text, (size_t)new_textLen * sizeof(char));
	Buf[textLen + new_textLen] = '\0';

	BufDirty = true;
	if (CursorPos >= pos)
		CursorPos += new_textLen;
	SelectionStart = SelectionEnd = CursorPos;
}

//Return false to discard a character.
static bool InputTextFilterCharacter(unsigned int* pChar, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* userData)
{
	unsigned int c = *pChar;

	if (c < 128 && c != ' ' && !isprint((int)(c & 0xFF)))
	{
		bool pass = false;
		pass |= (c == '\n' && (flags & ImGuiInputTextFlags_Multiline));
		pass |= (c == '\t' && (flags & ImGuiInputTextFlags_AllowTabInput));
		if (!pass)
			return false;
	}

	if (c >= 0xE000 && c <= 0xF8FF) //Filter private Unicode range. I don't imagine anybody would want to input them. GLFW on OSX seems to send private characters for special keys like arrow keys.
		return false;

	if (flags & (ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsNoBlank))
	{
		if (flags & ImGuiInputTextFlags_CharsDecimal)
		{
			if (!(c >= '0' && c <= '9') && (c != '.') && (c != '-') && (c != '+') && (c != '*') && (c != '/'))
				return false;
		}

		if (flags & ImGuiInputTextFlags_CharsHexadecimal)
		{
			if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F'))
				return false;
		}

		if (flags & ImGuiInputTextFlags_CharsUppercase)
		{
			if (c >= 'a' && c <= 'z')
				*pChar = (c += (unsigned int)('A' - 'a'));
		}

		if (flags & ImGuiInputTextFlags_CharsNoBlank)
		{
			if (ImCharIsSpace(c))
				return false;
		}
	}

	if (flags & ImGuiInputTextFlags_CallbackCharFilter)
	{
		ImGuiTextEditCallbackData callback_data;
		memset(&callback_data, 0, sizeof(ImGuiTextEditCallbackData));
		callback_data.EventFlag = ImGuiInputTextFlags_CallbackCharFilter;
		callback_data.EventChar = (ImWchar)c;
		callback_data.Flags = flags;
		callback_data.UserData = userData;
		if (callback(&callback_data) != 0)
			return false;
		*pChar = callback_data.EventChar;
		if (!callback_data.EventChar)
			return false;
	}

	return true;
}

//Edit a string of text
//FIXME: This is rather complex partly because we are doing UTF8 > u16 > UTF8 conversions on the go to more easily handle stb_textedit calls. Ideally we should stay in UTF-8 all the time. 
bool ImGui::InputTextEx(const char* label, char* buf, int bufSize, const ImVec2& sizeArg, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* userData)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackHistory) && (flags & ImGuiInputTextFlags_Multiline)));	//Can't use both together (they both use up/down keys)
	IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackCompletion) && (flags & ImGuiInputTextFlags_AllowTabInput)));	//Can't use both together (they both use tab key)

	ImGuiState& g = *GImGui;
	const ImGuiIO& io = g.IO;
	const ImGuiStyle& style = g.Style;

	const ImGuiID id = window->GetID(label);
	const bool is_multiline = (flags & ImGuiInputTextFlags_Multiline) != 0;
	const bool is_editable = (flags & ImGuiInputTextFlags_ReadOnly) == 0;

	ImVec2 labelSize = ImGui::CalcTextSize(label, NULL, true);
	ImVec2 size = CalcItemSize(sizeArg, CalcItemWidth(), is_multiline ? ImGui::GetTextLineHeight() * 8.f : labelSize.y);	//Arbitrary default of 8 lines high for multi-line
	const ImRect frameBB(window->DC.CursorPos, window->DC.CursorPos + size + style.FramePadding * 2.f);
	const ImRect totalBB(frameBB.Min, frameBB.Max + ImVec2(labelSize.x > 0.f ? (style.ItemInnerSpacing.x + labelSize.x) : 0.f, 0.f));

	ImGuiWindow* draw_window = window;
	if (is_multiline)
	{
		ImGui::BeginGroup();
		if (!ImGui::BeginChildFrame(id, frameBB.GetSize()))
		{
			ImGui::EndChildFrame();
			ImGui::EndGroup();
			return false;
		}
		draw_window = GetCurrentWindow();
		draw_window->DC.CursorPos += style.FramePadding;
		size.x -= draw_window->ScrollbarSizes.x;
	}
	else
	{
		ItemSize(totalBB, style.FramePadding.y);
		if (!ItemAdd(totalBB, &id))
			return false;
	}

	//NB: we are only allowed to access 'edit_state' if we are the active widget.
	ImGuiTextEditState& edit_state = g.InputTextState;

	const bool is_ctrl_down = io.KeyCtrl;
	const bool is_shift_down = io.KeyShift;
	const bool is_alt_down = io.KeyAlt;
	const bool focus_requested = FocusableItemRegister(window, g.ActiveId == id, (flags & (ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_AllowTabInput)) == 0);    //Using completion callback disable keyboard tabbing
	const bool focus_requested_by_code = focus_requested && (window->FocusIdxAllCounter == window->FocusIdxAllRequestCurrent);
	const bool focus_requested_by_tab = focus_requested && !focus_requested_by_code;

	const bool hovered = IsHovered(frameBB, id);
	if (hovered)
	{
		SetHoveredID(id);
		g.MouseCursor = ImGuiMouseCursor_TextInput;
	}
	const bool user_clicked = hovered && io.MouseClicked[0];
	const bool user_scrolled = is_multiline && g.ActiveId == 0 && edit_state.Id == id && g.ActiveIdPreviousFrame == draw_window->GetID("#SCROLLY");

	bool select_all = (g.ActiveId != id) && (flags & ImGuiInputTextFlags_AutoSelectAll) != 0;
	if (focus_requested || user_clicked || user_scrolled)
	{
		if (g.ActiveId != id)
		{
			//Start edition
			//Take a copy of the initial buffer value (both in original UTF-8 format and converted to wchar)
			//From the moment we focused we are ignoring the content of 'buf'
			const int prev_len_w = edit_state.CurLenW;
			edit_state.Text.resize(bufSize + 1);	//wchar count <= utf-8 count. we use +1 to make sure that .Data isn't NULL so it doesn't crash.
			edit_state.InitialText.resize(bufSize + 1);	//utf-8. we use +1 to make sure that .Data isn't NULL so it doesn't crash.
			ImFormatString(edit_state.InitialText.Data, edit_state.InitialText.Size, "%s", buf);
			const char* bufEnd = NULL;
			edit_state.CurLenW = ImTextStrFromUtf8(edit_state.Text.Data, edit_state.Text.Size, buf, NULL, &bufEnd);
			edit_state.CurLenA = (int)(bufEnd - buf);	//We can't get the result from ImFormatString() above because it is not UTF-8 aware. Here we'll cut off malformed UTF-8.
			edit_state.InputCursorScreenPos = ImVec2(-1.f, -1.f);
			edit_state.CursorAnimReset();

			//Preserve cursor position and undo/redo stack if we come back to same widget
			//FIXME: We should probably compare the whole buffer to be on the safety side. Comparing buf (utf8) and edit_state.Text (wchar).
			const bool recycle_state = (edit_state.Id == id) && (prev_len_w == edit_state.CurLenW);
			if (recycle_state)
			{
				//Recycle existing cursor/selection/undo stack but clamp position
				//Note a single mouse click will override the cursor/position immediately by calling stb_textedit_click handler.
				edit_state.CursorClamp();
			}
			else
			{
				edit_state.Id = id;
				edit_state.ScrollX = 0.f;
				stb_textedit_initialize_state(&edit_state.StbState, !is_multiline);
				if (!is_multiline && focus_requested_by_code)
					select_all = true;
			}
			if (flags & ImGuiInputTextFlags_AlwaysInsertMode)
				edit_state.StbState.insert_mode = true;
			if (!is_multiline && (focus_requested_by_tab || (user_clicked && is_ctrl_down)))
				select_all = true;
		}
		SetActiveID(id, window);
		FocusWindow(window);
	}
	else if (io.MouseClicked[0])
	{
		//Release focus when we click outside
		if (g.ActiveId == id)
			SetActiveID(0);
	}

	bool valueChanged = false;
	bool cancel_edit = false;
	bool enter_pressed = false;

	if (g.ActiveId == id)
	{
		if (!is_editable && !g.ActiveIdIsJustActivated)
		{
			//When read-only we always use the live data passed to the function
			edit_state.Text.resize(bufSize + 1);
			const char* bufEnd = NULL;
			edit_state.CurLenW = ImTextStrFromUtf8(edit_state.Text.Data, edit_state.Text.Size, buf, NULL, &bufEnd);
			edit_state.CurLenA = (int)(bufEnd - buf);
			edit_state.CursorClamp();
		}

		edit_state.BufSizeA = bufSize;

		//Although we are active we don't prevent mouse from hovering other elements unless we are interacting right now with the widget.
		//Down the line we should have a cleaner concept of focused vs active in the library.
		g.ActiveIdAllowHoveringOthers = !io.MouseDown[0];

		//Edit in progress
		const float mouse_x = (g.IO.MousePos.x - frameBB.Min.x - style.FramePadding.x) + edit_state.ScrollX;
		const float mouse_y = (is_multiline ? (g.IO.MousePos.y - draw_window->DC.CursorPos.y - style.FramePadding.y) : (g.FontSize * 0.5f));

		if (select_all || (hovered && io.MouseDoubleClicked[0]))
		{
			edit_state.SelectAll();
			edit_state.SelectedAllMouseLock = true;
		}
		else if (io.MouseClicked[0] && !edit_state.SelectedAllMouseLock)
		{
			stb_textedit_click(&edit_state, &edit_state.StbState, mouse_x, mouse_y);
			edit_state.CursorAnimReset();
		}
		else if (io.MouseDown[0] && !edit_state.SelectedAllMouseLock)
		{
			stb_textedit_drag(&edit_state, &edit_state.StbState, mouse_x, mouse_y);
			edit_state.CursorAnimReset();
			edit_state.CursorFollow = true;
		}
		if (edit_state.SelectedAllMouseLock && !io.MouseDown[0])
			edit_state.SelectedAllMouseLock = false;

		if (g.IO.InputCharacters[0])
		{
			//Process text input (before we check for Return because using some IME will effectively send a Return?)
			//We ignore CTRL inputs, but need to allow CTRL+ALT as some keyboards (e.g. German) use AltGR - which is Alt+Ctrl - to input certain characters.
			if (!(is_ctrl_down && !is_alt_down) && is_editable)
			{
				for (int n = 0; n < IM_ARRAYSIZE(g.IO.InputCharacters) && g.IO.InputCharacters[n]; n++)
				{
					if (unsigned int c = (unsigned int)g.IO.InputCharacters[n])
					{
						//Insert character if they pass filtering
						if (!InputTextFilterCharacter(&c, flags, callback, userData))
							continue;
						edit_state.OnKeyPressed((int)c);
					}
				}
			}

			//Consume characters
			memset(g.IO.InputCharacters, 0, sizeof(g.IO.InputCharacters));
		}

		//Handle various key-presses
		const int k_mask = (is_shift_down ? STB_TEXTEDIT_K_SHIFT : 0);
		const bool is_ctrl_only = is_ctrl_down && !is_alt_down && !is_shift_down;
		if (IsKeyPressedMap(ImGuiKey_LeftArrow))
			edit_state.OnKeyPressed(is_ctrl_down ? STB_TEXTEDIT_K_WORDLEFT | k_mask : STB_TEXTEDIT_K_LEFT | k_mask);
		else if (IsKeyPressedMap(ImGuiKey_RightArrow))
			edit_state.OnKeyPressed(is_ctrl_down ? STB_TEXTEDIT_K_WORDRIGHT | k_mask : STB_TEXTEDIT_K_RIGHT | k_mask);
		else if (is_multiline && IsKeyPressedMap(ImGuiKey_UpArrow))
		{
			if (is_ctrl_down)
				SetWindowScrollY(draw_window, draw_window->Scroll.y - g.FontSize);
			else
				edit_state.OnKeyPressed(STB_TEXTEDIT_K_UP | k_mask);
		}
		else if (is_multiline && IsKeyPressedMap(ImGuiKey_DownArrow))
		{
			if (is_ctrl_down)
				SetWindowScrollY(draw_window, draw_window->Scroll.y + g.FontSize);
			else
				edit_state.OnKeyPressed(STB_TEXTEDIT_K_DOWN | k_mask);
		}
		else if (IsKeyPressedMap(ImGuiKey_Home))
			edit_state.OnKeyPressed(is_ctrl_down ? STB_TEXTEDIT_K_TEXTSTART | k_mask : STB_TEXTEDIT_K_LINESTART | k_mask);
		else if (IsKeyPressedMap(ImGuiKey_End))
			edit_state.OnKeyPressed(is_ctrl_down ? STB_TEXTEDIT_K_TEXTEND | k_mask : STB_TEXTEDIT_K_LINEEND | k_mask);
		else if (IsKeyPressedMap(ImGuiKey_Delete) && is_editable)
			edit_state.OnKeyPressed(STB_TEXTEDIT_K_DELETE | k_mask);
		else if (IsKeyPressedMap(ImGuiKey_Backspace) && is_editable)
			edit_state.OnKeyPressed(STB_TEXTEDIT_K_BACKSPACE | k_mask);
		else if (IsKeyPressedMap(ImGuiKey_Enter))
		{
			bool ctrl_enter_for_new_line = (flags & ImGuiInputTextFlags_CtrlEnterForNewLine) != 0;
			if (!is_multiline || (ctrl_enter_for_new_line && !is_ctrl_down) || (!ctrl_enter_for_new_line && is_ctrl_down))
			{
				SetActiveID(0);
				enter_pressed = true;
			}
			else if (is_editable)
			{
				unsigned int c = '\n';	//Insert new line
				if (InputTextFilterCharacter(&c, flags, callback, userData))
					edit_state.OnKeyPressed((int)c);
			}
		}
		else if ((flags & ImGuiInputTextFlags_AllowTabInput) && IsKeyPressedMap(ImGuiKey_Tab) && !is_ctrl_down && !is_shift_down && !is_alt_down && is_editable)
		{
			unsigned int c = '\t';	//Insert TAB
			if (InputTextFilterCharacter(&c, flags, callback, userData))
				edit_state.OnKeyPressed((int)c);
		}
		else if (IsKeyPressedMap(ImGuiKey_Escape))
		{
			SetActiveID(0);
			cancel_edit = true;
		}
		else if (is_ctrl_only && IsKeyPressedMap(ImGuiKey_Z) && is_editable)
		{
			edit_state.OnKeyPressed(STB_TEXTEDIT_K_UNDO);
			edit_state.ClearSelection();
		}
		else if (is_ctrl_only && IsKeyPressedMap(ImGuiKey_Y) && is_editable)
		{
			edit_state.OnKeyPressed(STB_TEXTEDIT_K_REDO);
			edit_state.ClearSelection();
		}
		else if (is_ctrl_only && IsKeyPressedMap(ImGuiKey_A))
		{
			edit_state.SelectAll();
			edit_state.CursorFollow = true;
		}
		else if (is_ctrl_only && ((IsKeyPressedMap(ImGuiKey_X) && is_editable) || IsKeyPressedMap(ImGuiKey_C)) && (!is_multiline || edit_state.HasSelection()))
		{
			//Cut, Copy
			const bool cut = IsKeyPressedMap(ImGuiKey_X);
			if (cut && !edit_state.HasSelection())
				edit_state.SelectAll();

			if (g.IO.SetClipboardTextFn)
			{
				const int ib = edit_state.HasSelection() ? ImMin(edit_state.StbState.select_start, edit_state.StbState.select_end) : 0;
				const int ie = edit_state.HasSelection() ? ImMax(edit_state.StbState.select_start, edit_state.StbState.select_end) : edit_state.CurLenW;
				edit_state.TempTextBuffer.resize((ie - ib) * 4 + 1);
				ImTextStrToUtf8(edit_state.TempTextBuffer.Data, edit_state.TempTextBuffer.Size, edit_state.Text.Data + ib, edit_state.Text.Data + ie);
				g.IO.SetClipboardTextFn(edit_state.TempTextBuffer.Data);
			}

			if (cut)
			{
				edit_state.CursorFollow = true;
				stb_textedit_cut(&edit_state, &edit_state.StbState);
			}
		}
		else if (is_ctrl_only && IsKeyPressedMap(ImGuiKey_V) && is_editable)
		{
			//Paste
			if (g.IO.GetClipboardTextFn)
			{
				if (const char* clipboard = g.IO.GetClipboardTextFn())
				{
					//Remove new-line from pasted buffer
					const int clipboard_len = (int)strlen(clipboard);
					ImWchar* clipboard_filtered = (ImWchar*)ImGui::MemAlloc((clipboard_len + 1) * sizeof(ImWchar));
					int clipboard_filtered_len = 0;
					for (const char* s = clipboard; *s; )
					{
						unsigned int c;
						s += ImTextCharFromUtf8(&c, s, NULL);
						if (c == 0)
							break;
						if (c >= 0x10000 || !InputTextFilterCharacter(&c, flags, callback, userData))
							continue;
						clipboard_filtered[clipboard_filtered_len++] = (ImWchar)c;
					}
					clipboard_filtered[clipboard_filtered_len] = 0;
					if (clipboard_filtered_len > 0)	//If everything was filtered, ignore the pasting operation
					{
						stb_textedit_paste(&edit_state, &edit_state.StbState, clipboard_filtered, clipboard_filtered_len);
						edit_state.CursorFollow = true;
					}
					ImGui::MemFree(clipboard_filtered);
				}
			}
		}

		if (cancel_edit)
		{
			//Restore initial value
			if (is_editable)
			{
				ImFormatString(buf, bufSize, "%s", edit_state.InitialText.Data);
				valueChanged = true;
			}
		}
		else
		{
			//Apply new value immediately - copy modified buffer back
			//Note that as soon as we can focus into the input box, the in-widget value gets priority over any underlying modification of the input buffer
			//FIXME: We actually always render 'buf' when calling DrawList->AddText
			//FIXME-OPT: CPU waste to do this every time the widget is active, should mark dirty state from the stb_textedit callbacks
			if (is_editable)
			{
				edit_state.TempTextBuffer.resize(edit_state.Text.Size * 4);
				ImTextStrToUtf8(edit_state.TempTextBuffer.Data, edit_state.TempTextBuffer.Size, edit_state.Text.Data, NULL);
			}

			//User callback
			if ((flags & (ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackAlways)) != 0)
			{
				IM_ASSERT(callback != NULL);

				//The reason we specify the usage semantic (Completion/History) is that Completion needs to disable keyboard TABBING at the moment.
				ImGuiInputTextFlags event_flag = 0;
				ImGuiKey event_key = ImGuiKey_COUNT;
				if ((flags & ImGuiInputTextFlags_CallbackCompletion) != 0 && IsKeyPressedMap(ImGuiKey_Tab))
				{
					event_flag = ImGuiInputTextFlags_CallbackCompletion;
					event_key = ImGuiKey_Tab;
				}
				else if ((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && IsKeyPressedMap(ImGuiKey_UpArrow))
				{
					event_flag = ImGuiInputTextFlags_CallbackHistory;
					event_key = ImGuiKey_UpArrow;
				}
				else if ((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && IsKeyPressedMap(ImGuiKey_DownArrow))
				{
					event_flag = ImGuiInputTextFlags_CallbackHistory;
					event_key = ImGuiKey_DownArrow;
				}

				if (event_key != ImGuiKey_COUNT || (flags & ImGuiInputTextFlags_CallbackAlways) != 0)
				{
					ImGuiTextEditCallbackData callback_data;
					callback_data.EventFlag = event_flag;
					callback_data.Flags = flags;
					callback_data.UserData = userData;
					callback_data.ReadOnly = !is_editable;

					callback_data.EventKey = event_key;
					callback_data.Buf = edit_state.TempTextBuffer.Data;
					callback_data.BufSize = edit_state.BufSizeA;
					callback_data.BufDirty = false;

					//We have to convert from position from wchar to UTF-8 positions
					ImWchar* text = edit_state.Text.Data;
					const int utf8_cursor_pos = callback_data.CursorPos = ImTextCountUtf8BytesFromStr(text, text + edit_state.StbState.cursor);
					const int utf8_selection_start = callback_data.SelectionStart = ImTextCountUtf8BytesFromStr(text, text + edit_state.StbState.select_start);
					const int utf8_selection_end = callback_data.SelectionEnd = ImTextCountUtf8BytesFromStr(text, text + edit_state.StbState.select_end);

					//Call user code
					callback(&callback_data);

					//Read back what user may have modified
					IM_ASSERT(callback_data.Buf == edit_state.TempTextBuffer.Data);	//Invalid to modify those fields
					IM_ASSERT(callback_data.BufSize == edit_state.BufSizeA);
					IM_ASSERT(callback_data.Flags == flags);
					if (callback_data.CursorPos != utf8_cursor_pos)
						edit_state.StbState.cursor = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.CursorPos);
					if (callback_data.SelectionStart != utf8_selection_start)
						edit_state.StbState.select_start = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionStart);
					if (callback_data.SelectionEnd != utf8_selection_end)
						edit_state.StbState.select_end = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionEnd);
					if (callback_data.BufDirty)
					{
						edit_state.CurLenW = ImTextStrFromUtf8(text, edit_state.Text.Size, edit_state.TempTextBuffer.Data, NULL);
						edit_state.CurLenA = (int)strlen(edit_state.TempTextBuffer.Data);
						edit_state.CursorAnimReset();
					}
				}
			}

			//Copy back to user buffer
			if (is_editable && strcmp(edit_state.TempTextBuffer.Data, buf) != 0)
			{
				ImFormatString(buf, bufSize, "%s", edit_state.TempTextBuffer.Data);
				valueChanged = true;
			}
		}
	}

	if (!is_multiline)
		RenderFrame(frameBB.Min, frameBB.Max, window->Color(ImGuiCol_FrameBg), true, style.FrameRounding);

	//Render
	const ImVec4 clipRect(frameBB.Min.x, frameBB.Min.y, frameBB.Min.x + size.x + style.FramePadding.x * 2.f, frameBB.Min.y + size.y + style.FramePadding.y * 2.f);
	ImVec2 render_pos = is_multiline ? draw_window->DC.CursorPos : frameBB.Min + style.FramePadding;
	ImVec2 textSize(0.f, 0.f);
	if (g.ActiveId == id || (edit_state.Id == id && is_multiline && g.ActiveId == draw_window->GetID("#SCROLLY")))
	{
		edit_state.CursorAnim += g.IO.DeltaTime;

		//We need to:
		//- Display the text (this can be more easily clipped)
		//- Handle scrolling, highlight selection, display cursor (those all requires some form of 1d->2d cursor position calculation)
		//- Measure text height (for scrollbar)
		//We are attempting to do most of that in **one main pass** to minimize the computation cost (non-negligible for large amount of text) + 2nd pass for selection rendering (we could merge them by an extra refactoring effort)
		const ImWchar* textBegin = edit_state.Text.Data;
		ImVec2 cursor_offset;
		ImVec2 select_start_offset;

		{
			//Count lines + find lines numbers straddling 'cursor' and 'select_start' position.
			const ImWchar* searches_input_ptr[2];
			searches_input_ptr[0] = textBegin + edit_state.StbState.cursor;
			searches_input_ptr[1] = NULL;
			int searches_remaining = 1;
			int searches_result_line_number[2] = { -1, -999 };
			if (edit_state.StbState.select_start != edit_state.StbState.select_end)
			{
				searches_input_ptr[1] = textBegin + ImMin(edit_state.StbState.select_start, edit_state.StbState.select_end);
				searches_result_line_number[1] = -1;
				searches_remaining++;
			}

			//Iterate all lines to find our line numbers
			//In multi-line mode, we never exit the loop until all lines are counted, so add one extra to the searches_remaining counter.
			searches_remaining += is_multiline ? 1 : 0;
			int line_count = 0;
			for (const ImWchar* s = textBegin; *s != 0; s++)
			{
				if (*s == '\n')
				{
					line_count++;
					if (searches_result_line_number[0] == -1 && s >= searches_input_ptr[0])
					{
						searches_result_line_number[0] = line_count;
						if (--searches_remaining <= 0)
							break;
					}
					if (searches_result_line_number[1] == -1 && s >= searches_input_ptr[1])
					{
						searches_result_line_number[1] = line_count;
						if (--searches_remaining <= 0)
							break;
					}
				}
			}
			line_count++;
			if (searches_result_line_number[0] == -1)
				searches_result_line_number[0] = line_count;
			if (searches_result_line_number[1] == -1)
				searches_result_line_number[1] = line_count;

			//Calculate 2d position by finding the beginning of the line and measuring distance
			cursor_offset.x = InputTextCalcTextSizeW(ImStrbolW(searches_input_ptr[0], textBegin), searches_input_ptr[0]).x;
			cursor_offset.y = searches_result_line_number[0] * g.FontSize;
			if (searches_result_line_number[1] >= 0)
			{
				select_start_offset.x = InputTextCalcTextSizeW(ImStrbolW(searches_input_ptr[1], textBegin), searches_input_ptr[1]).x;
				select_start_offset.y = searches_result_line_number[1] * g.FontSize;
			}

			//Calculate text height
			if (is_multiline)
				textSize = ImVec2(size.x, line_count * g.FontSize);
		}

		//Scroll
		if (edit_state.CursorFollow)
		{
			//Horizontal scroll in chunks of quarter width
			if (!(flags & ImGuiInputTextFlags_NoHorizontalScroll))
			{
				const float scroll_increment_x = size.x * 0.25f;
				if (cursor_offset.x < edit_state.ScrollX)
					edit_state.ScrollX = (float)(int)ImMax(0.f, cursor_offset.x - scroll_increment_x);
				else if (cursor_offset.x - size.x >= edit_state.ScrollX)
					edit_state.ScrollX = (float)(int)(cursor_offset.x - size.x + scroll_increment_x);
			}
			else
				edit_state.ScrollX = 0.f;

			//Vertical scroll
			if (is_multiline)
			{
				float scrollY = draw_window->Scroll.y;
				if (cursor_offset.y - g.FontSize < scrollY)
					scrollY = ImMax(0.f, cursor_offset.y - g.FontSize);
				else if (cursor_offset.y - size.y >= scrollY)
					scrollY = cursor_offset.y - size.y;
				draw_window->DC.CursorPos.y += (draw_window->Scroll.y - scrollY);	//To avoid a frame of lag
				draw_window->Scroll.y = scrollY;
				render_pos.y = draw_window->DC.CursorPos.y;
			}
		}
		edit_state.CursorFollow = false;
		const ImVec2 render_scroll = ImVec2(edit_state.ScrollX, 0.f);

		//Draw selection
		if (edit_state.StbState.select_start != edit_state.StbState.select_end)
		{
			const ImWchar* text_selected_begin = textBegin + ImMin(edit_state.StbState.select_start, edit_state.StbState.select_end);
			const ImWchar* text_selected_end = textBegin + ImMax(edit_state.StbState.select_start, edit_state.StbState.select_end);

			float bg_offy_up = is_multiline ? 0.f : -1.f;	//FIXME: those offsets should be part of the style? they don't play so well with multi-line selection.
			float bg_offy_dn = is_multiline ? 0.f : 2.f;
			ImU32 bgColor = draw_window->Color(ImGuiCol_TextSelectedBg);
			ImVec2 rect_pos = render_pos + select_start_offset - render_scroll;
			for (const ImWchar* p = text_selected_begin; p < text_selected_end; )
			{
				if (rect_pos.y > clipRect.w + g.FontSize)
					break;
				if (rect_pos.y < clipRect.y)
				{
					while (p < text_selected_end)
					{
						if (*p++ == '\n')
							break;
					}
				}
				else
				{
					ImVec2 rect_size = InputTextCalcTextSizeW(p, text_selected_end, &p, NULL, true);
					if (rect_size.x <= 0.f)
						rect_size.x = (float)(int)(g.Font->GetCharAdvance((unsigned short)' ') * 0.5f);
					//So we can see selected empty lines
					ImRect rect(rect_pos + ImVec2(0.f, bg_offy_up - g.FontSize), rect_pos + ImVec2(rect_size.x, bg_offy_dn));
					rect.Clip(clipRect);
					if (rect.Overlaps(clipRect))
						draw_window->DrawList->AddRectFilled(rect.Min, rect.Max, bgColor);
				}
				rect_pos.x = render_pos.x - render_scroll.x;
				rect_pos.y += g.FontSize;
			}
		}

		draw_window->DrawList->AddText(g.Font, g.FontSize, render_pos - render_scroll, draw_window->Color(ImGuiCol_Text), buf, buf + edit_state.CurLenA, 0.f, is_multiline ? NULL : &clipRect);

		//Draw blinking cursor
		ImVec2 cursor_screenPos = render_pos + cursor_offset - render_scroll;
		bool cursor_is_visible = (g.InputTextState.CursorAnim <= 0.f) || fmodf(g.InputTextState.CursorAnim, 1.2f) <= 0.8f;
		if (cursor_is_visible)
			draw_window->DrawList->AddLine(cursor_screenPos + ImVec2(0.f, -g.FontSize + 0.5f), cursor_screenPos + ImVec2(0.f, -1.5f), window->Color(ImGuiCol_Text));

		//Notify OS of text input position for advanced IME
		if (is_editable && io.ImeSetInputScreenPosFn && ImLengthSqr(edit_state.InputCursorScreenPos - cursor_screenPos) > 0.0001f)
			io.ImeSetInputScreenPosFn((int)cursor_screenPos.x - 1, (int)(cursor_screenPos.y - g.FontSize));	//-1 x offset so that Windows IME can cover our cursor. Bit of an extra nicety.

		edit_state.InputCursorScreenPos = cursor_screenPos;
	}
	else
	{
		//Render text only
		const char* bufEnd = NULL;
		if (is_multiline)
			textSize = ImVec2(size.x, InputTextCalcTextLenAndLineCount(buf, &bufEnd) * g.FontSize);	//We don't need width
		draw_window->DrawList->AddText(g.Font, g.FontSize, render_pos, draw_window->Color(ImGuiCol_Text), buf, bufEnd, 0.f, is_multiline ? NULL : &clipRect);
	}

	if (is_multiline)
	{
		ImGui::Dummy(textSize + ImVec2(0.f, g.FontSize));	//Always add room to scroll an extra line
		ImGui::EndChildFrame();
		ImGui::EndGroup();
	}

	//Log as text
	if (g.LogEnabled)
		LogRenderedText(render_pos, buf, NULL);

	if (labelSize.x > 0)
		RenderText(ImVec2(frameBB.Max.x + style.ItemInnerSpacing.x, frameBB.Min.y + style.FramePadding.y), label);

	if ((flags & ImGuiInputTextFlags_EnterReturnsTrue) != 0)
		return enter_pressed;
	else
		return valueChanged;
}

bool ImGui::InputText(const char* label, char* buf, size_t bufSize, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* userData)
{
	IM_ASSERT(!(flags & ImGuiInputTextFlags_Multiline));	//call InputTextMultiline()
	bool ret = InputTextEx(label, buf, (int)bufSize, ImVec2(0, 0), flags, callback, userData);
	return ret;
}

bool ImGui::InputTextMultiline(const char* label, char* buf, size_t bufSize, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* userData)
{
	bool ret = InputTextEx(label, buf, (int)bufSize, size, flags | ImGuiInputTextFlags_Multiline, callback, userData);
	return ret;
}

//NB: scalarFormat here must be a simple "%xx" format string with no prefix/suffix (unlike the Drag/Slider functions "displayFormat" argument)
bool ImGui::InputScalarEx(const char* label, ImGuiDataType dataType, void* dataPtr, void* step_ptr, void* step_fast_ptr, const char* scalarFormat, ImGuiInputTextFlags extraFlags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const float w = CalcItemWidth();
	const ImVec2 labelSize = ImGui::CalcTextSize(label, NULL, true);
	const ImRect frameBB(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, labelSize.y) + style.FramePadding * 2.f);

	ImGui::BeginGroup();
	ImGui::PushID(label);
	const ImVec2 button_sz = ImVec2(g.FontSize, g.FontSize) + style.FramePadding * 2;
	if (step_ptr)
		ImGui::PushItemWidth(ImMax(1.f, w - (button_sz.x + style.ItemInnerSpacing.x) * 2));

	char buf[64];
	DataTypeFormatString(dataType, dataPtr, scalarFormat, buf, IM_ARRAYSIZE(buf));

	bool valueChanged = false;
	if (!(extraFlags & ImGuiInputTextFlags_CharsHexadecimal))
		extraFlags |= ImGuiInputTextFlags_CharsDecimal;
	extraFlags |= ImGuiInputTextFlags_AutoSelectAll;
	if (ImGui::InputText("", buf, IM_ARRAYSIZE(buf), extraFlags))
	{
		DataTypeApplyOpFromText(buf, GImGui->InputTextState.InitialText.begin(), dataType, dataPtr, scalarFormat);
		valueChanged = true;
	}

	//Step buttons
	if (step_ptr)
	{
		ImGui::PopItemWidth();
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		if (ButtonEx("-", button_sz, ImGuiButtonFlags_Repeat | ImGuiButtonFlags_DontClosePopups))
		{
			DataTypeApplyOp(dataType, '-', dataPtr, g.IO.KeyCtrl && step_fast_ptr ? step_fast_ptr : step_ptr);
			valueChanged = true;
		}
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		if (ButtonEx("+", button_sz, ImGuiButtonFlags_Repeat | ImGuiButtonFlags_DontClosePopups))
		{
			DataTypeApplyOp(dataType, '+', dataPtr, g.IO.KeyCtrl && step_fast_ptr ? step_fast_ptr : step_ptr);
			valueChanged = true;
		}
	}
	ImGui::PopID();

	if (labelSize.x > 0)
	{
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		RenderText(ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + style.FramePadding.y), label);
		ItemSize(labelSize, style.FramePadding.y);
	}
	ImGui::EndGroup();

	return valueChanged;
}

bool ImGui::InputFloat(const char* label, float* v, float step, float step_fast, int decimalPrecision, ImGuiInputTextFlags extraFlags)
{
	char displayFormat[16];
	if (decimalPrecision < 0)
		strcpy(displayFormat, "%f");	//Ideally we'd have a minimum decimal precision of 1 to visually denote that this is a float, while hiding non-significant digits? %f doesn't have a minimum of 1
	else
		ImFormatString(displayFormat, 16, "%%.%df", decimalPrecision);
	return InputScalarEx(label, ImGuiDataType_Float, (void*)v, (void*)(step>0.f ? &step : NULL), (void*)(step_fast > 0.f ? &step_fast : NULL), displayFormat, extraFlags);
}

bool ImGui::InputInt(const char* label, int* v, int step, int step_fast, ImGuiInputTextFlags extraFlags)
{
	//Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use InputText() to parse your own data, if you want to handle prefixes.
	const char* scalarFormat = (extraFlags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
	return InputScalarEx(label, ImGuiDataType_Int, (void*)v, (void*)(step > 0.f ? &step : NULL), (void*)(step_fast > 0.f ? &step_fast : NULL), scalarFormat, extraFlags);
}

bool ImGui::InputFloatN(const char* label, float* v, int components, int decimalPrecision, ImGuiInputTextFlags extraFlags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	bool valueChanged = false;
	ImGui::BeginGroup();
	ImGui::PushID(label);
	PushMultiItemsWidths(components);
	for (int i = 0; i < components; i++)
	{
		ImGui::PushID(i);
		valueChanged |= ImGui::InputFloat("##v", &v[i], 0, 0, decimalPrecision, extraFlags);
		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		ImGui::PopID();
		ImGui::PopItemWidth();
	}
	ImGui::PopID();

	window->DC.CurrentLineTextBaseOffset = ImMax(window->DC.CurrentLineTextBaseOffset, g.Style.FramePadding.y);
	ImGui::TextUnformatted(label, FindTextDisplayEnd(label));
	ImGui::EndGroup();

	return valueChanged;
}

bool ImGui::InputFloat2(const char* label, float v[2], int decimalPrecision, ImGuiInputTextFlags extraFlags)
{
	return InputFloatN(label, v, 2, decimalPrecision, extraFlags);
}

bool ImGui::InputFloat3(const char* label, float v[3], int decimalPrecision, ImGuiInputTextFlags extraFlags)
{
	return InputFloatN(label, v, 3, decimalPrecision, extraFlags);
}

bool ImGui::InputFloat4(const char* label, float v[4], int decimalPrecision, ImGuiInputTextFlags extraFlags)
{
	return InputFloatN(label, v, 4, decimalPrecision, extraFlags);
}

bool ImGui::InputIntN(const char* label, int* v, int components, ImGuiInputTextFlags extraFlags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	bool valueChanged = false;
	ImGui::BeginGroup();
	ImGui::PushID(label);
	PushMultiItemsWidths(components);
	for (int i = 0; i < components; i++)
	{
		ImGui::PushID(i);
		valueChanged |= ImGui::InputInt("##v", &v[i], 0, 0, extraFlags);
		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		ImGui::PopID();
		ImGui::PopItemWidth();
	}
	ImGui::PopID();

	window->DC.CurrentLineTextBaseOffset = ImMax(window->DC.CurrentLineTextBaseOffset, g.Style.FramePadding.y);
	ImGui::TextUnformatted(label, FindTextDisplayEnd(label));
	ImGui::EndGroup();

	return valueChanged;
}

bool ImGui::InputInt2(const char* label, int v[2], ImGuiInputTextFlags extraFlags)
{
	return InputIntN(label, v, 2, extraFlags);
}

bool ImGui::InputInt3(const char* label, int v[3], ImGuiInputTextFlags extraFlags)
{
	return InputIntN(label, v, 3, extraFlags);
}

bool ImGui::InputInt4(const char* label, int v[4], ImGuiInputTextFlags extraFlags)
{
	return InputIntN(label, v, 4, extraFlags);
}

static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
	const char** items = (const char**)data;
	if (out_text)
		*out_text = items[idx];
	return true;
}

static bool Items_SingleStringGetter(void* data, int idx, const char** out_text)
{
	//FIXME-OPT: we could pre-compute the indices to fasten this. But only 1 active combo means the waste is limited.
	const char* items_separated_by_zeros = (const char*)data;
	int itemsCount = 0;
	const char* p = items_separated_by_zeros;
	while (*p)
	{
		if (idx == itemsCount)
			break;
		p += strlen(p) + 1;
		itemsCount++;
	}
	if (!*p)
		return false;
	if (out_text)
		*out_text = p;
	return true;
}

//Combo box helper allowing to pass an array of strings.
bool ImGui::Combo(const char* label, int* current_item, const char** items, int itemsCount, int height_in_items)
{
	const bool valueChanged = Combo(label, current_item, Items_ArrayGetter, (void*)items, itemsCount, height_in_items);
	return valueChanged;
}

//Combo box helper allowing to pass all items in a single string.
bool ImGui::Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items)
{
	int itemsCount = 0;
	const char* p = items_separated_by_zeros;	//FIXME-OPT: Avoid computing this, or at least only when combo is open
	while (*p)
	{
		p += strlen(p) + 1;
		itemsCount++;
	}
	bool valueChanged = Combo(label, current_item, Items_SingleStringGetter, (void*)items_separated_by_zeros, itemsCount, height_in_items);
	return valueChanged;
}

//Combo box function.
bool ImGui::Combo(const char* label, int* current_item, bool(*items_getter)(void*, int, const char**), void* data, int itemsCount, int height_in_items)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const float w = CalcItemWidth();

	const ImVec2 labelSize = CalcTextSize(label, NULL, true);
	const ImRect frameBB(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, labelSize.y) + style.FramePadding * 2.f);
	const ImRect totalBB(frameBB.Min, frameBB.Max + ImVec2(labelSize.x > 0.f ? style.ItemInnerSpacing.x + labelSize.x : 0.f, 0.f));
	ItemSize(totalBB, style.FramePadding.y);
	if (!ItemAdd(totalBB, &id))
		return false;

	const float arrow_size = (g.FontSize + style.FramePadding.x * 2.f);
	const bool hovered = IsHovered(frameBB, id);

	const ImRect valueBB(frameBB.Min, frameBB.Max - ImVec2(arrow_size, 0.f));
	RenderFrame(frameBB.Min, frameBB.Max, window->Color(ImGuiCol_FrameBg), true, style.FrameRounding);
	RenderFrame(ImVec2(frameBB.Max.x - arrow_size, frameBB.Min.y), frameBB.Max, window->Color(hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button), true, style.FrameRounding);	//FIXME-ROUNDING
	RenderCollapseTriangle(ImVec2(frameBB.Max.x - arrow_size, frameBB.Min.y) + style.FramePadding, true);

	if (*current_item >= 0 && *current_item < itemsCount)
	{
		const char* item_text;
		if (items_getter(data, *current_item, &item_text))
			RenderTextClipped(frameBB.Min + style.FramePadding, valueBB.Max, item_text, NULL, NULL);
	}

	if (labelSize.x > 0)
		RenderText(ImVec2(frameBB.Max.x + style.ItemInnerSpacing.x, frameBB.Min.y + style.FramePadding.y), label);

	bool menu_toggled = false;
	if (hovered)
	{
		SetHoveredID(id);
		if (g.IO.MouseClicked[0])
		{
			SetActiveID(0);
			if (IsPopupOpen(id))
				ClosePopup(id);
			else
			{
				FocusWindow(window);
				ImGui::OpenPopup(label);
				menu_toggled = true;
			}
		}
	}

	bool valueChanged = false;
	if (IsPopupOpen(id))
	{
		//Size default to hold ~7 items
		if (height_in_items < 0)
			height_in_items = 7;

		float popup_height = (labelSize.y + style.ItemSpacing.y) * ImMin(itemsCount, height_in_items) + (style.FramePadding.y * 3);
		ImRect popup_rect(ImVec2(frameBB.Min.x, frameBB.Max.y), ImVec2(frameBB.Max.x, frameBB.Max.y + popup_height));
		popup_rect.Max.y = ImMin(popup_rect.Max.y, g.IO.DisplaySize.y - style.DisplaySafeAreaPadding.y);	//Adhoc height limit for Combo. Ideally should be handled in Begin() along with other popups size, we want to have the possibility of moving the popup above as well.
		ImGui::SetNextWindowPos(popup_rect.Min);
		ImGui::SetNextWindowSize(popup_rect.GetSize());
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, style.FramePadding);

		const ImGuiWindowFlags flags = ImGuiWindowFlags_ComboBox | ((window->Flags & ImGuiWindowFlags_ShowBorders) ? ImGuiWindowFlags_ShowBorders : 0);
		if (BeginPopupEx(label, flags))
		{
			//Display items
			ImGui::Spacing();
			for (int i = 0; i < itemsCount; i++)
			{
				ImGui::PushID((void*)(intptr_t)i);
				const bool item_selected = (i == *current_item);
				const char* item_text;
				if (!items_getter(data, i, &item_text))
					item_text = "*Unknown item*";
				if (ImGui::Selectable(item_text, item_selected))
				{
					SetActiveID(0);
					valueChanged = true;
					*current_item = i;
				}
				if (item_selected && menu_toggled)
					ImGui::SetScrollHere();
				ImGui::PopID();
			}
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();
	}
	return valueChanged;
}

//Tip: pass an empty label (e.g. "##dummy") then you can use the space to draw other text or image.
//But you need to make sure the ID is unique, e.g. enclose calls in PushID/PopID.
bool ImGui::Selectable(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& sizeArg)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	if ((flags & ImGuiSelectableFlags_SpanAllColumns) && window->DC.ColumnsCount > 1)
		PopClipRect();

	ImGuiID id = window->GetID(label);
	ImVec2 labelSize = ImGui::CalcTextSize(label, NULL, true);
	ImVec2 size(sizeArg.x != 0.f ? sizeArg.x : labelSize.x, sizeArg.y != 0.f ? sizeArg.y : labelSize.y);
	ImVec2 pos = window->DC.CursorPos;
	pos.y += window->DC.CurrentLineTextBaseOffset;
	ImRect bb(pos, pos + size);
	ItemSize(bb);

	//Fill horizontal space.
	ImVec2 windowPadding = window->WindowPadding;
	float max_x = (flags & ImGuiSelectableFlags_SpanAllColumns) ? ImGui::GetWindowContentRegionMax().x : ImGui::GetContentRegionMax().x;
	float w_draw = ImMax(labelSize.x, window->Pos.x + max_x - windowPadding.x - window->DC.CursorPos.x);
	ImVec2 size_draw((sizeArg.x != 0 && !(flags & ImGuiSelectableFlags_DrawFillAvailWidth)) ? sizeArg.x : w_draw, sizeArg.y != 0.f ? sizeArg.y : size.y);
	ImRect bb_with_spacing(pos, pos + size_draw);
	if (sizeArg.x == 0.f || (flags & ImGuiSelectableFlags_DrawFillAvailWidth))
		bb_with_spacing.Max.x += windowPadding.x;

	//Selectables are tightly packed together, we extend the box to cover spacing between selectable.
	float spacing_L = (float)(int)(style.ItemSpacing.x * 0.5f);
	float spacing_U = (float)(int)(style.ItemSpacing.y * 0.5f);
	float spacing_R = style.ItemSpacing.x - spacing_L;
	float spacing_D = style.ItemSpacing.y - spacing_U;
	bb_with_spacing.Min.x -= spacing_L;
	bb_with_spacing.Min.y -= spacing_U;
	bb_with_spacing.Max.x += spacing_R;
	bb_with_spacing.Max.y += spacing_D;
	if (!ItemAdd(bb_with_spacing, &id))
	{
		if ((flags & ImGuiSelectableFlags_SpanAllColumns) && window->DC.ColumnsCount > 1)
			PushColumnClipRect();
		return false;
	}

	ImGuiButtonFlags button_flags = 0;
	if (flags & ImGuiSelectableFlags_Menu)
		button_flags |= ImGuiButtonFlags_PressedOnClick;
	if (flags & ImGuiSelectableFlags_MenuItem)
		button_flags |= ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_PressedOnRelease;
	if (flags & ImGuiSelectableFlags_Disabled)
		button_flags |= ImGuiButtonFlags_Disabled;
	bool hovered, held;
	bool pressed = ButtonBehavior(bb_with_spacing, id, &hovered, &held, button_flags);
	if (flags & ImGuiSelectableFlags_Disabled)
		selected = false;

	//Render
	if (hovered || selected)
	{
		const ImU32 col = window->Color((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
		RenderFrame(bb_with_spacing.Min, bb_with_spacing.Max, col, false, 0.f);
	}

	if ((flags & ImGuiSelectableFlags_SpanAllColumns) && window->DC.ColumnsCount > 1)
	{
		PushColumnClipRect();
		bb_with_spacing.Max.x -= (ImGui::GetContentRegionMax().x - max_x);
	}

	if (flags & ImGuiSelectableFlags_Disabled)
		ImGui::PushStyleColor(ImGuiCol_Text, g.Style.Colors[ImGuiCol_TextDisabled]);
	RenderTextClipped(bb.Min, bb_with_spacing.Max, label, NULL, &labelSize);
	if (flags & ImGuiSelectableFlags_Disabled)
		ImGui::PopStyleColor();

	//Automatically close popups
	if (pressed && !(flags & ImGuiSelectableFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
		ImGui::CloseCurrentPopup();
	return pressed;
}

bool ImGui::Selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags, const ImVec2& sizeArg)
{
	if (ImGui::Selectable(label, *p_selected, flags, sizeArg))
	{
		*p_selected = !*p_selected;
		return true;
	}
	return false;
}

//Helper to calculate the size of a listbox and display a label on the right.
//Tip: To have a list filling the entire window width, PushItemWidth(-1) and pass an empty label "##empty"
bool ImGui::ListBoxHeader(const char* label, const ImVec2& sizeArg)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	const ImGuiStyle& style = ImGui::GetStyle();
	const ImGuiID id = ImGui::GetID(label);
	const ImVec2 labelSize = ImGui::CalcTextSize(label, NULL, true);

	//Size default to hold ~7 items. Fractional number of items helps seeing that we can scroll down/up without looking at scrollbar.
	ImVec2 size = CalcItemSize(sizeArg, CalcItemWidth() + style.FramePadding.x * 2.f, ImGui::GetTextLineHeightWithSpacing() * 7.4f + style.ItemSpacing.y);
	ImVec2 frame_size = ImVec2(size.x, ImMax(size.y, labelSize.y));
	ImRect frameBB(window->DC.CursorPos, window->DC.CursorPos + frame_size);
	ImRect bb(frameBB.Min, frameBB.Max + ImVec2(labelSize.x > 0.f ? style.ItemInnerSpacing.x + labelSize.x : 0.f, 0.f));
	window->DC.LastItemRect = bb;

	ImGui::BeginGroup();
	if (labelSize.x > 0)
		RenderText(ImVec2(frameBB.Max.x + style.ItemInnerSpacing.x, frameBB.Min.y + style.FramePadding.y), label);

	ImGui::BeginChildFrame(id, frameBB.GetSize());
	return true;
}

bool ImGui::ListBoxHeader(const char* label, int itemsCount, int height_in_items)
{
	//Size default to hold ~7 items. Fractional number of items helps seeing that we can scroll down/up without looking at scrollbar.
	//However we don't add +0.4f if itemsCount <= height_in_items. It is slightly dodgy, because it means a dynamic list of items will make the widget resize occasionally when it crosses that size.
	//I am expecting that someone will come and complain about this behavior in a remote future, then we can advise on a better solution.
	if (height_in_items < 0)
		height_in_items = ImMin(itemsCount, 7);
	float height_in_items_f = height_in_items < itemsCount ? (height_in_items + 0.4f) : (height_in_items + 0.0f);

	//We include ItemSpacing.y so that a list sized for the exact number of items doesn't make a scrollbar appears. We could also enforce that by passing a flag to BeginChild().
	ImVec2 size;
	size.x = 0.f;
	size.y = ImGui::GetTextLineHeightWithSpacing() * height_in_items_f + ImGui::GetStyle().ItemSpacing.y;
	return ImGui::ListBoxHeader(label, size);
}

void ImGui::ListBoxFooter()
{
	ImGuiWindow* parentWindow = GetParentWindow();
	const ImRect bb = parentWindow->DC.LastItemRect;
	const ImGuiStyle& style = ImGui::GetStyle();

	ImGui::EndChildFrame();

	//Redeclare item size so that it includes the label (we have stored the full size in LastItemRect)
	//We call SameLine() to restore DC.CurrentLine* data
	ImGui::SameLine();
	parentWindow->DC.CursorPos = bb.Min;
	ItemSize(bb, style.FramePadding.y);
	ImGui::EndGroup();
}

bool ImGui::ListBox(const char* label, int* current_item, const char** items, int itemsCount, int height_items)
{
	const bool valueChanged = ListBox(label, current_item, Items_ArrayGetter, (void*)items, itemsCount, height_items);
	return valueChanged;
}

bool ImGui::ListBox(const char* label, int* current_item, bool(*items_getter)(void*, int, const char**), void* data, int itemsCount, int height_in_items)
{
	if (!ImGui::ListBoxHeader(label, itemsCount, height_in_items))
		return false;

	//Assume all items have even height (= 1 line of text). If you need items of different or variable sizes you can create a custom version of ListBox() in your code without using the clipper.
	bool valueChanged = false;
	ImGuiListClipper clipper(itemsCount, ImGui::GetTextLineHeightWithSpacing());
	for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
	{
		const bool item_selected = (i == *current_item);
		const char* item_text;
		if (!items_getter(data, i, &item_text))
			item_text = "*Unknown item*";

		ImGui::PushID(i);
		if (ImGui::Selectable(item_text, item_selected))
		{
			*current_item = i;
			valueChanged = true;
		}
		ImGui::PopID();
	}
	clipper.End();
	ImGui::ListBoxFooter();
	return valueChanged;
}

bool ImGui::MenuItem(const char* label, const char* shortcut, bool selected, bool enabled)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	ImVec2 pos = window->DC.CursorPos;
	ImVec2 labelSize = CalcTextSize(label, NULL, true);
	ImVec2 shortcut_size = shortcut ? CalcTextSize(shortcut, NULL) : ImVec2(0.f, 0.f);
	float w = window->MenuColumns.DeclColumns(labelSize.x, shortcut_size.x, (float)(int)(g.FontSize * 1.2f));	//Feedback for next frame
	float extra_w = ImMax(0.f, ImGui::GetContentRegionAvail().x - w);

	bool pressed = ImGui::Selectable(label, false, ImGuiSelectableFlags_MenuItem | ImGuiSelectableFlags_DrawFillAvailWidth | (!enabled ? ImGuiSelectableFlags_Disabled : 0), ImVec2(w, 0.f));
	if (shortcut_size.x > 0.f)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, g.Style.Colors[ImGuiCol_TextDisabled]);
		RenderText(pos + ImVec2(window->MenuColumns.Pos[1] + extra_w, 0.f), shortcut, NULL, false);
		ImGui::PopStyleColor();
	}

	if (selected)
		RenderCheckMark(pos + ImVec2(window->MenuColumns.Pos[2] + extra_w + g.FontSize * 0.2f, 0.f), window->Color(ImGuiCol_Text));

	return pressed;
}

bool ImGui::MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled)
{
	if (ImGui::MenuItem(label, shortcut, p_selected ? *p_selected : false, enabled))
	{
		if (p_selected)
			*p_selected = !*p_selected;
		return true;
	}
	return false;
}

bool ImGui::BeginMainMenuBar()
{
	ImGuiState& g = *GImGui;
	ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
	ImGui::SetNextWindowSize(ImVec2(g.IO.DisplaySize.x, g.FontBaseSize + g.Style.FramePadding.y * 2.f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
	if (!ImGui::Begin("##MainMenuBar", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar)
		|| !ImGui::BeginMenuBar())
	{
		ImGui::End();
		ImGui::PopStyleVar(2);
		return false;
	}
	g.CurrentWindow->DC.MenuBarOffsetX += g.Style.DisplaySafeAreaPadding.x;
	return true;
}

void ImGui::EndMainMenuBar()
{
	ImGui::EndMenuBar();
	ImGui::End();
	ImGui::PopStyleVar(2);
}

bool ImGui::BeginMenuBar()
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;
	if (!(window->Flags & ImGuiWindowFlags_MenuBar))
		return false;

	IM_ASSERT(!window->DC.MenuBarAppending);
	ImGui::BeginGroup();	//Save position
	ImGui::PushID("##menubar");
	ImRect rect = window->MenuBarRect();
	PushClipRect(ImVec4(rect.Min.x + 0.5f, rect.Min.y - 0.5f, rect.Max.x + 0.5f, rect.Max.y - 1.5f), false);
	window->DC.CursorPos = ImVec2(rect.Min.x + window->DC.MenuBarOffsetX, rect.Min.y);	//+ g.Style.FramePadding.y);
	window->DC.LayoutType = ImGuiLayoutType_Horizontal;
	window->DC.MenuBarAppending = true;
	ImGui::AlignFirstTextHeightToWidgets();
	return true;
}

void ImGui::EndMenuBar()
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	IM_ASSERT(window->Flags & ImGuiWindowFlags_MenuBar);
	IM_ASSERT(window->DC.MenuBarAppending);
	PopClipRect();
	ImGui::PopID();
	window->DC.MenuBarOffsetX = window->DC.CursorPos.x - window->MenuBarRect().Min.x;
	window->DC.GroupStack.back().AdvanceCursor = false;
	ImGui::EndGroup();
	window->DC.LayoutType = ImGuiLayoutType_Vertical;
	window->DC.MenuBarAppending = false;
}

bool ImGui::BeginMenu(const char* label, bool enabled)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	ImVec2 labelSize = CalcTextSize(label, NULL, true);
	ImGuiWindow* backed_focused_window = g.FocusedWindow;

	bool pressed;
	bool opened = IsPopupOpen(id);
	bool menuset_opened = !(window->Flags & ImGuiWindowFlags_Popup) && (g.OpenedPopupStack.Size > g.CurrentPopupStack.Size && g.OpenedPopupStack[g.CurrentPopupStack.Size].ParentMenuSet == window->GetID("##menus"));
	if (menuset_opened)
		g.FocusedWindow = window;

	ImVec2 popup_pos, pos = window->DC.CursorPos;
	if (window->DC.LayoutType == ImGuiLayoutType_Horizontal)
	{
		popup_pos = ImVec2(pos.x - window->WindowPadding.x, pos.y - style.FramePadding.y + window->MenuBarHeight());
		window->DC.CursorPos.x += (float)(int)(style.ItemSpacing.x * 0.5f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, style.ItemSpacing * 2.f);
		float w = labelSize.x;
		pressed = ImGui::Selectable(label, opened, ImGuiSelectableFlags_Menu | ImGuiSelectableFlags_DontClosePopups | (!enabled ? ImGuiSelectableFlags_Disabled : 0), ImVec2(w, 0.f));
		ImGui::PopStyleVar();
		ImGui::SameLine();
		window->DC.CursorPos.x += (float)(int)(style.ItemSpacing.x * 0.5f);
	}
	else
	{
		popup_pos = ImVec2(pos.x, pos.y - style.WindowPadding.y);
		float w = window->MenuColumns.DeclColumns(labelSize.x, 0.f, (float)(int)(g.FontSize * 1.2f));	//Feedback to next frame
		float extra_w = ImMax(0.f, ImGui::GetContentRegionAvail().x - w);
		pressed = ImGui::Selectable(label, opened, ImGuiSelectableFlags_Menu | ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_DrawFillAvailWidth | (!enabled ? ImGuiSelectableFlags_Disabled : 0), ImVec2(w, 0.f));
		if (!enabled)
			ImGui::PushStyleColor(ImGuiCol_Text, g.Style.Colors[ImGuiCol_TextDisabled]);
		RenderCollapseTriangle(pos + ImVec2(window->MenuColumns.Pos[2] + extra_w + g.FontSize * 0.2f, 0.f), false);
		if (!enabled)
			ImGui::PopStyleColor();
	}

	bool hovered = enabled && IsHovered(window->DC.LastItemRect, id);
	if (menuset_opened)
		g.FocusedWindow = backed_focused_window;

	bool want_open = false, want_close = false;
	if (window->Flags & (ImGuiWindowFlags_Popup | ImGuiWindowFlags_ChildMenu))
	{
		//Implement http://bjk5.com/post/44698559168/breaking-down-amazons-mega-dropdown to avoid using timers so menus feel more reactive.
		bool moving_within_opened_triangle = false;
		if (g.HoveredWindow == window && g.OpenedPopupStack.Size > g.CurrentPopupStack.Size && g.OpenedPopupStack[g.CurrentPopupStack.Size].ParentWindow == window)
		{
			if (ImGuiWindow* next_window = g.OpenedPopupStack[g.CurrentPopupStack.Size].Window)
			{
				ImRect next_windowRect = next_window->Rect();
				ImVec2 ta = g.IO.MousePos - g.IO.MouseDelta;
				ImVec2 tb = (window->Pos.x < next_window->Pos.x) ? next_windowRect.GetTL() : next_windowRect.GetTR();
				ImVec2 tc = (window->Pos.x < next_window->Pos.x) ? next_windowRect.GetBL() : next_windowRect.GetBR();
				float extra = ImClamp(fabsf(ta.x - tb.x) * 0.3f, 5.f, 30.f);	//add a bit of extra slack.
				ta.x += (window->Pos.x < next_window->Pos.x) ? -0.5f : +0.5f;	//to avoid numerical issues
				tb.y = ta.y + ImMax((tb.y - extra) - ta.y, -100.f);	//triangle is maximum 200 high to limit the slope and the bias toward large sub-menus
				tc.y = ta.y + ImMin((tc.y + extra) - ta.y, +100.f);
				moving_within_opened_triangle = ImIsPointInTriangle(g.IO.MousePos, ta, tb, tc);
				//window->DrawList->PushClipRectFullScreen();
				//window->DrawList->AddTriangleFilled(ta, tb, tc, moving_within_opened_triangle ? 0x80008000 : 0x80000080);
				//window->DrawList->PopClipRect();	//Debug
			}
		}

		want_close = (opened && !hovered && g.HoveredWindow == window && g.HoveredIdPreviousFrame != 0 && g.HoveredIdPreviousFrame != id && !moving_within_opened_triangle);
		want_open = (!opened && hovered && !moving_within_opened_triangle) || (!opened && hovered && pressed);
	}
	else if (opened && pressed && menuset_opened) //menu-bar: click open menu to close
	{
		want_close = true;
		want_open = opened = false;
	}
	else if (pressed || (hovered && menuset_opened && !opened)) //menu-bar: first click to open, then hover to open others
		want_open = true;

	if (want_close && IsPopupOpen(id))
		ClosePopupToLevel(GImGui->CurrentPopupStack.Size);

	if (!opened && want_open && g.OpenedPopupStack.Size > g.CurrentPopupStack.Size)
	{
		//Don't recycle same menu level in the same frame, first close the other menu and yield for a frame.
		ImGui::OpenPopup(label);
		return false;
	}

	opened |= want_open;
	if (want_open)
		ImGui::OpenPopup(label);

	if (opened)
	{
		ImGui::SetNextWindowPos(popup_pos, ImGuiSetCond_Always);
		ImGuiWindowFlags flags = ImGuiWindowFlags_ShowBorders | ((window->Flags & (ImGuiWindowFlags_Popup | ImGuiWindowFlags_ChildMenu)) ? ImGuiWindowFlags_ChildMenu | ImGuiWindowFlags_ChildWindow : ImGuiWindowFlags_ChildMenu);
		opened = BeginPopupEx(label, flags);	//opened can be 'false' when the popup is completely clipped (e.g. zero size display)
	}

	return opened;
}

void ImGui::EndMenu()
{
	ImGui::EndPopup();
}

//A little colored square. Return true when clicked.
//FIXME: May want to display/ignore the alpha component in the color display? Yet show it in the tooltip.
bool ImGui::ColorButton(const ImVec4& col, bool small_height, bool outline_border)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID("#colorbutton");
	const float square_size = g.FontSize;
	const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(square_size + style.FramePadding.y * 2, square_size + (small_height ? 0 : style.FramePadding.y * 2)));
	ItemSize(bb, small_height ? 0.f : style.FramePadding.y);
	if (!ItemAdd(bb, &id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held);
	RenderFrame(bb.Min, bb.Max, window->Color(col), outline_border, style.FrameRounding);

	if (hovered)
		ImGui::SetTooltip("Color:\n(%.2f,%.2f,%.2f,%.2f)\n#%02X%02X%02X%02X", col.x, col.y, col.z, col.w, IM_F32_TO_INT8(col.x), IM_F32_TO_INT8(col.y), IM_F32_TO_INT8(col.z), IM_F32_TO_INT8(col.z));

	return pressed;
}

bool ImGui::ColorEdit3(const char* label, float col[3])
{
	float col4[4];
	col4[0] = col[0];
	col4[1] = col[1];
	col4[2] = col[2];
	col4[3] = 1.f;
	const bool valueChanged = ImGui::ColorEdit4(label, col4, false);
	col[0] = col4[0];
	col[1] = col4[1];
	col[2] = col4[2];
	return valueChanged;
}

//Edit colors components (each component in 0.f..1.f range
//Use CTRL-Click to input value and TAB to go to next item.
bool ImGui::ColorEdit4(const char* label, float col[4], bool alpha)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiState& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const float wFull = CalcItemWidth();
	const float square_sz = (g.FontSize + style.FramePadding.y * 2.f);

	ImGuiColorEditMode edit_mode = window->DC.ColorEditMode;
	if (edit_mode == ImGuiColorEditMode_UserSelect || edit_mode == ImGuiColorEditMode_UserSelectShowButton)
		edit_mode = g.ColorEditModeStorage.GetInt(id, 0) % 3;

	float f[4] = { col[0], col[1], col[2], col[3] };

	if (edit_mode == ImGuiColorEditMode_HSV)
		ImGui::ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

	int i[4] = { IM_F32_TO_INT8(f[0]), IM_F32_TO_INT8(f[1]), IM_F32_TO_INT8(f[2]), IM_F32_TO_INT8(f[3]) };

	int components = alpha ? 4 : 3;
	bool valueChanged = false;

	ImGui::BeginGroup();
	ImGui::PushID(label);

	const bool hsv = (edit_mode == 1);
	switch (edit_mode)
	{
	case ImGuiColorEditMode_RGB:
	case ImGuiColorEditMode_HSV:
	{
		//RGB/HSV 0..255 Sliders
		const float w_items_all = wFull - (square_sz + style.ItemInnerSpacing.x);
		const float wItemOne = ImMax(1.f, (float)(int)((w_items_all - (style.FramePadding.x * 2.f + style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
		const float wItemLast = ImMax(1.f, (float)(int)(w_items_all - (wItemOne + style.FramePadding.x * 2.f + style.ItemInnerSpacing.x) * (components - 1)));

		const bool hide_prefix = (wItemOne <= CalcTextSize("M:999").x);
		const char* ids[4] = { "##X", "##Y", "##Z", "##W" };
		const char* fmt_table[3][4] =
		{
			{   "%3.f",   "%3.f",   "%3.f",   "%3.f" },
			{ "R:%3.f", "G:%3.f", "B:%3.f", "A:%3.f" },
			{ "H:%3.f", "S:%3.f", "V:%3.f", "A:%3.f" }
		};
		const char** fmt = hide_prefix ? fmt_table[0] : hsv ? fmt_table[2] : fmt_table[1];

		ImGui::PushItemWidth(wItemOne);
		for (int n = 0; n < components; n++)
		{
			if (n > 0)
				ImGui::SameLine(0, style.ItemInnerSpacing.x);
			if (n + 1 == components)
				ImGui::PushItemWidth(wItemLast);
			valueChanged |= ImGui::DragInt(ids[n], &i[n], 1.f, 0, 255, fmt[n]);
		}
		ImGui::PopItemWidth();
		ImGui::PopItemWidth();
	}
	break;
	case ImGuiColorEditMode_HEX:
	{
		//RGB Hexadecimal Input
		const float w_slider_all = wFull - square_sz;
		char buf[64];
		if (alpha)
			ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", i[0], i[1], i[2], i[3]);
		else
			ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", i[0], i[1], i[2]);
		ImGui::PushItemWidth(w_slider_all - style.ItemInnerSpacing.x);
		valueChanged |= ImGui::InputText("##Text", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
		ImGui::PopItemWidth();
		char* p = buf;
		while (*p == '#' || ImCharIsSpace(*p))
			p++;

		//Treat at unsigned (%X is unsigned)
		i[0] = i[1] = i[2] = i[3] = 0;
		if (alpha)
			sscanf(p, "%02X%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2], (unsigned int*)&i[3]);
		else
			sscanf(p, "%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2]);
	}
	break;
	}

	ImGui::SameLine(0, style.ItemInnerSpacing.x);

	const ImVec4 col_display(col[0], col[1], col[2], 1.f);
	if (ImGui::ColorButton(col_display))
		g.ColorEditModeStorage.SetInt(id, (edit_mode + 1) % 3);	//Don't set local copy of 'edit_mode' right away!

	//Recreate our own tooltip over's ColorButton() one because we want to display correct alpha here
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Color:\n(%.2f,%.2f,%.2f,%.2f)\n#%02X%02X%02X%02X", col[0], col[1], col[2], col[3], IM_F32_TO_INT8(col[0]), IM_F32_TO_INT8(col[1]), IM_F32_TO_INT8(col[2]), IM_F32_TO_INT8(col[3]));

	if (window->DC.ColorEditMode == ImGuiColorEditMode_UserSelectShowButton)
	{
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		const char* button_titles[3] = { "RGB", "HSV", "HEX" };
		if (ButtonEx(button_titles[edit_mode], ImVec2(0, 0), ImGuiButtonFlags_DontClosePopups))
			g.ColorEditModeStorage.SetInt(id, (edit_mode + 1) % 3);	//Don't set local copy of 'edit_mode' right away!
		ImGui::SameLine();
	}
	else
		ImGui::SameLine(0, style.ItemInnerSpacing.x);

	ImGui::TextUnformatted(label, FindTextDisplayEnd(label));

	//Convert back
	for (int n = 0; n < 4; n++)
		f[n] = i[n] / 255.f;
	if (edit_mode == 1)
		ImGui::ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);

	if (valueChanged)
	{
		col[0] = f[0];
		col[1] = f[1];
		col[2] = f[2];
		if (alpha)
			col[3] = f[3];
	}

	ImGui::PopID();
	ImGui::EndGroup();

	return valueChanged;
}

void ImGui::ColorEditMode(ImGuiColorEditMode mode)
{
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.ColorEditMode = mode;
}

//Horizontal separating line.
void ImGui::Separator()
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	if (window->DC.ColumnsCount > 1)
		PopClipRect();

	float x1 = window->Pos.x;
	float x2 = window->Pos.x + window->Size.x;
	if (!window->DC.GroupStack.empty())
		x1 += window->DC.ColumnsStartX;

	const ImRect bb(ImVec2(x1, window->DC.CursorPos.y), ImVec2(x2, window->DC.CursorPos.y));
	ItemSize(ImVec2(0.f, 0.f));	//NB: we don't provide our width so that it doesn't get feed back into AutoFit   //FIXME: Height should be 1.f not 0.f ?
	if (!ItemAdd(bb, NULL))
	{
		if (window->DC.ColumnsCount > 1)
			PushColumnClipRect();
		return;
	}

	window->DrawList->AddLine(bb.Min, bb.Max, window->Color(ImGuiCol_Border));

	ImGuiState& g = *GImGui;
	if (g.LogEnabled)
		ImGui::LogText(IM_NEWLINE "--------------------------------");

	if (window->DC.ColumnsCount > 1)
	{
		PushColumnClipRect();
		window->DC.ColumnsCellMinY = window->DC.CursorPos.y;
	}
}

void ImGui::Spacing()
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;
	ItemSize(ImVec2(0, 0));
}

void ImGui::Dummy(const ImVec2& size)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
	ItemSize(bb);
	ItemAdd(bb, NULL);
}

bool ImGui::IsRectVisible(const ImVec2& size)
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return window->ClipRect.Overlaps(ImRect(window->DC.CursorPos, window->DC.CursorPos + size));
}

void ImGui::BeginGroup()
{
	ImGuiWindow* window = GetCurrentWindow();

	window->DC.GroupStack.resize(window->DC.GroupStack.Size + 1);
	ImGuiGroupData& group_data = window->DC.GroupStack.back();
	group_data.BackupCursorPos = window->DC.CursorPos;
	group_data.BackupCursorMaxPos = window->DC.CursorMaxPos;
	group_data.BackupColumnsStartX = window->DC.ColumnsStartX;
	group_data.BackupCurrentLineHeight = window->DC.CurrentLineHeight;
	group_data.BackupCurrentLineTextBaseOffset = window->DC.CurrentLineTextBaseOffset;
	group_data.BackupLogLinePosY = window->DC.LogLinePosY;
	group_data.AdvanceCursor = true;

	window->DC.ColumnsStartX = window->DC.CursorPos.x - window->Pos.x;
	window->DC.CursorMaxPos = window->DC.CursorPos;
	window->DC.CurrentLineHeight = 0.f;
	window->DC.LogLinePosY = window->DC.CursorPos.y - 9999.f;
}

void ImGui::EndGroup()
{
	ImGuiWindow* window = GetCurrentWindow();
	ImGuiStyle& style = ImGui::GetStyle();

	IM_ASSERT(!window->DC.GroupStack.empty());

	ImGuiGroupData& group_data = window->DC.GroupStack.back();

	ImRect group_bb(group_data.BackupCursorPos, window->DC.CursorMaxPos);
	group_bb.Max.y -= style.ItemSpacing.y;	//Cancel out last vertical spacing because we are adding one ourselves.
	group_bb.Max = ImMax(group_bb.Min, group_bb.Max);

	window->DC.CursorPos = group_data.BackupCursorPos;
	window->DC.CursorMaxPos = ImMax(group_data.BackupCursorMaxPos, window->DC.CursorMaxPos);
	window->DC.CurrentLineHeight = group_data.BackupCurrentLineHeight;
	window->DC.CurrentLineTextBaseOffset = group_data.BackupCurrentLineTextBaseOffset;
	window->DC.ColumnsStartX = group_data.BackupColumnsStartX;
	window->DC.LogLinePosY = window->DC.CursorPos.y - 9999.f;

	if (group_data.AdvanceCursor)
	{
		window->DC.CurrentLineTextBaseOffset = ImMax(window->DC.PrevLineTextBaseOffset, group_data.BackupCurrentLineTextBaseOffset);	//FIXME: Incorrect, we should grab the base offset from the *first line* of the group but it is hard to obtain now.
		ItemSize(group_bb.GetSize(), group_data.BackupCurrentLineTextBaseOffset);
		ItemAdd(group_bb, NULL);
	}

	window->DC.GroupStack.pop_back();

	//window->DrawList->AddRect(group_bb.Min, group_bb.Max, 0xFFFF00FF);	//Debug
}

//Gets back to previous line and continue with horizontal layout
//     local_pos_x == 0   : follow on previous item
//     local_pos_x != 0   : align to specified column
//     spacing_w < 0   : use default spacing if column_x==0, no spacing if column_x!=0
//     spacing_w >= 0  : enforce spacing
void ImGui::SameLine(float local_pos_x, float spacing_w)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiState& g = *GImGui;
	float x, y;
	if (local_pos_x != 0.f)
	{
		if (spacing_w < 0.f)
			spacing_w = 0.f;
		x = window->Pos.x - window->Scroll.x + local_pos_x + spacing_w;
		y = window->DC.CursorPosPrevLine.y;
	}
	else
	{
		if (spacing_w < 0.f)
			spacing_w = g.Style.ItemSpacing.x;
		x = window->DC.CursorPosPrevLine.x + spacing_w;
		y = window->DC.CursorPosPrevLine.y;
	}
	window->DC.CurrentLineHeight = window->DC.PrevLineHeight;
	window->DC.CurrentLineTextBaseOffset = window->DC.PrevLineTextBaseOffset;
	window->DC.CursorPos = ImVec2(x, y);
}

void ImGui::NextColumn()
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiState& g = *GImGui;
	if (window->DC.ColumnsCount > 1)
	{
		ImGui::PopItemWidth();
		PopClipRect();

		window->DC.ColumnsCellMaxY = ImMax(window->DC.ColumnsCellMaxY, window->DC.CursorPos.y);
		if (++window->DC.ColumnsCurrent < window->DC.ColumnsCount)
		{
			window->DC.ColumnsOffsetX = ImGui::GetColumnOffset(window->DC.ColumnsCurrent) - window->DC.ColumnsStartX + g.Style.ItemSpacing.x;
			window->DrawList->ChannelsSetCurrent(window->DC.ColumnsCurrent);
		}
		else
		{
			window->DC.ColumnsCurrent = 0;
			window->DC.ColumnsOffsetX = 0.f;
			window->DC.ColumnsCellMinY = window->DC.ColumnsCellMaxY;
			window->DrawList->ChannelsSetCurrent(0);
		}
		window->DC.CursorPos.x = (float)(int)(window->Pos.x + window->DC.ColumnsStartX + window->DC.ColumnsOffsetX);
		window->DC.CursorPos.y = window->DC.ColumnsCellMinY;
		window->DC.CurrentLineHeight = 0.f;
		window->DC.CurrentLineTextBaseOffset = 0.f;

		PushColumnClipRect();
		ImGui::PushItemWidth(ImGui::GetColumnWidth() * 0.65f);	//FIXME
	}
}

int ImGui::GetColumnIndex()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return window->DC.ColumnsCurrent;
}

int ImGui::GetColumnsCount()
{
	ImGuiWindow* window = GetCurrentWindowRead();
	return window->DC.ColumnsCount;
}

static float GetDraggedColumnOffset(int columnIndex)
{
	//Active (dragged) column always follow mouse. The reason we need this is that dragging a column to the right edge of an auto-resizing
	//window creates a feedback loop because we store normalized positions/ So while dragging we enforce absolute positioning
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = ImGui::GetCurrentWindowRead();
	IM_ASSERT(columnIndex > 0);	//We cannot drag column 0. If you get this assert you may have a conflict between the ID of your columns and another widgets.
	IM_ASSERT(g.ActiveId == window->DC.ColumnsSetID + ImGuiID(columnIndex));

	float x = g.IO.MousePos.x + g.ActiveClickDeltaToCenter.x - window->Pos.x;
	x = ImClamp(x, ImGui::GetColumnOffset(columnIndex - 1) + g.Style.ColumnsMinSpacing, ImGui::GetColumnOffset(columnIndex + 1) - g.Style.ColumnsMinSpacing);

	return (float)(int)x;
}

float ImGui::GetColumnOffset(int columnIndex)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindowRead();
	if (columnIndex < 0)
		columnIndex = window->DC.ColumnsCurrent;

	if (g.ActiveId)
	{
		const ImGuiID column_id = window->DC.ColumnsSetID + ImGuiID(columnIndex);
		if (g.ActiveId == column_id)
			return GetDraggedColumnOffset(columnIndex);
	}

	//Read from cache
	IM_ASSERT(columnIndex < window->DC.ColumnsOffsetsT.Size);
	const float t = window->DC.ColumnsOffsetsT[columnIndex];

	const float content_region_width = window->SizeContentsExplicit.x ? window->SizeContentsExplicit.x : window->Size.x;
	const float min_x = window->DC.ColumnsStartX;
	const float max_x = content_region_width - window->Scroll.x - ((window->Flags & ImGuiWindowFlags_NoScrollbar) ? 0 : g.Style.ScrollbarSize);	//- window->WindowPadding().x;
	const float x = min_x + t * (max_x - min_x);
	return (float)(int)x;
}

void ImGui::SetColumnOffset(int columnIndex, float offset)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();
	if (columnIndex < 0)
		columnIndex = window->DC.ColumnsCurrent;

	IM_ASSERT(columnIndex < window->DC.ColumnsOffsetsT.Size);
	const ImGuiID column_id = window->DC.ColumnsSetID + ImGuiID(columnIndex);

	const float content_region_width = window->SizeContentsExplicit.x ? window->SizeContentsExplicit.x : window->Size.x;
	const float min_x = window->DC.ColumnsStartX;
	const float max_x = content_region_width - window->Scroll.x - ((window->Flags & ImGuiWindowFlags_NoScrollbar) ? 0 : g.Style.ScrollbarSize);	//- window->WindowPadding().x;
	const float t = (offset - min_x) / (max_x - min_x);
	window->DC.StateStorage->SetFloat(column_id, t);
	window->DC.ColumnsOffsetsT[columnIndex] = t;
}

float ImGui::GetColumnWidth(int columnIndex)
{
	ImGuiWindow* window = GetCurrentWindowRead();
	if (columnIndex < 0)
		columnIndex = window->DC.ColumnsCurrent;

	const float w = GetColumnOffset(columnIndex + 1) - GetColumnOffset(columnIndex);
	return w;
}

static void PushColumnClipRect(int columnIndex)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (columnIndex < 0)
		columnIndex = window->DC.ColumnsCurrent;

	const float x1 = window->Pos.x + ImGui::GetColumnOffset(columnIndex) - 1;
	const float x2 = window->Pos.x + ImGui::GetColumnOffset(columnIndex + 1) - 1;
	PushClipRect(ImVec4(x1, -FLT_MAX, x2, +FLT_MAX));
}

void ImGui::Columns(int columns_count, const char* id, bool border)
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();

	if (window->DC.ColumnsCount != 1)
	{
		if (window->DC.ColumnsCurrent != 0)
			ItemSize(ImVec2(0, 0));	//Advance to column 0
		ImGui::PopItemWidth();
		PopClipRect();
		window->DrawList->ChannelsMerge();

		window->DC.ColumnsCellMaxY = ImMax(window->DC.ColumnsCellMaxY, window->DC.CursorPos.y);
		window->DC.CursorPos.y = window->DC.ColumnsCellMaxY;
	}

	//Draw columns borders and handle resize at the time of "closing" a columns set
	if (window->DC.ColumnsCount != columns_count && window->DC.ColumnsCount != 1 && window->DC.ColumnsShowBorders && !window->SkipItems)
	{
		const float y1 = window->DC.ColumnsStartPos.y;
		const float y2 = window->DC.CursorPos.y;
		for (int i = 1; i < window->DC.ColumnsCount; i++)
		{
			float x = window->Pos.x + GetColumnOffset(i);
			const ImGuiID column_id = window->DC.ColumnsSetID + ImGuiID(i);
			const ImRect column_rect(ImVec2(x - 4, y1), ImVec2(x + 4, y2));
			if (IsClippedEx(column_rect, &column_id, false))
				continue;

			bool hovered, held;
			ButtonBehavior(column_rect, column_id, &hovered, &held, true);
			if (hovered || held)
				g.MouseCursor = ImGuiMouseCursor_ResizeEW;

			//Draw before resize so our items positioning are in sync with the line being drawn
			const ImU32 col = window->Color(held ? ImGuiCol_ColumnActive : hovered ? ImGuiCol_ColumnHovered : ImGuiCol_Column);
			const float xi = (float)(int)x;
			window->DrawList->AddLine(ImVec2(xi, y1 + 1.f), ImVec2(xi, y2), col);

			if (held)
			{
				if (g.ActiveIdIsJustActivated)
					g.ActiveClickDeltaToCenter.x = x - g.IO.MousePos.x;

				x = GetDraggedColumnOffset(i);
				SetColumnOffset(i, x);
			}
		}
	}

	//Set state for first column
	ImGui::PushID(0x11223344);	//Differentiate column ID with an arbitrary/random prefix for cases where users name their columns set the same as another non-scope widget
	window->DC.ColumnsSetID = window->GetID(id ? id : "");
	ImGui::PopID();
	window->DC.ColumnsCurrent = 0;
	window->DC.ColumnsCount = columns_count;
	window->DC.ColumnsShowBorders = border;
	window->DC.ColumnsStartPos = window->DC.CursorPos;
	window->DC.ColumnsCellMinY = window->DC.ColumnsCellMaxY = window->DC.CursorPos.y;
	window->DC.ColumnsOffsetX = 0.f;
	window->DC.CursorPos.x = (float)(int)(window->Pos.x + window->DC.ColumnsStartX + window->DC.ColumnsOffsetX);

	if (window->DC.ColumnsCount != 1)
	{
		//Cache column offsets
		window->DC.ColumnsOffsetsT.resize(columns_count + 1);
		for (int columnIndex = 0; columnIndex < columns_count + 1; columnIndex++)
		{
			const ImGuiID column_id = window->DC.ColumnsSetID + ImGuiID(columnIndex);
			KeepAliveID(column_id);
			const float default_t = columnIndex / (float)window->DC.ColumnsCount;
			const float t = window->DC.StateStorage->GetFloat(column_id, default_t);	//Cheaply store our floating point value inside the integer (could store an union into the map?)
			window->DC.ColumnsOffsetsT[columnIndex] = t;
		}
		window->DrawList->ChannelsSplit(window->DC.ColumnsCount);
		PushColumnClipRect();
		ImGui::PushItemWidth(ImGui::GetColumnWidth() * 0.65f);
	}
	else
	{
		window->DC.ColumnsOffsetsT.resize(2);
		window->DC.ColumnsOffsetsT[0] = 0.f;
		window->DC.ColumnsOffsetsT[1] = 1.f;
	}
}

void ImGui::Indent()
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.ColumnsStartX += g.Style.IndentSpacing;
	window->DC.CursorPos.x = window->Pos.x + window->DC.ColumnsStartX + window->DC.ColumnsOffsetX;
}

void ImGui::Unindent()
{
	ImGuiState& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();
	window->DC.ColumnsStartX -= g.Style.IndentSpacing;
	window->DC.CursorPos.x = window->Pos.x + window->DC.ColumnsStartX + window->DC.ColumnsOffsetX;
}

void ImGui::TreePush(const char* strID)
{
	ImGuiWindow* window = GetCurrentWindow();
	ImGui::Indent();
	window->DC.TreeDepth++;
	PushID(strID ? strID : "#TreePush");
}

void ImGui::TreePush(const void* ptrID)
{
	ImGuiWindow* window = GetCurrentWindow();
	ImGui::Indent();
	window->DC.TreeDepth++;
	PushID(ptrID ? ptrID : (const void*)"#TreePush");
}

void ImGui::TreePop()
{
	ImGuiWindow* window = GetCurrentWindow();
	ImGui::Unindent();
	window->DC.TreeDepth--;
	PopID();
}

void ImGui::Value(const char* prefix, bool b)
{
	ImGui::Text("%s: %s", prefix, (b ? "true" : "false"));
}

void ImGui::Value(const char* prefix, int v)
{
	ImGui::Text("%s: %d", prefix, v);
}

void ImGui::Value(const char* prefix, unsigned int v)
{
	ImGui::Text("%s: %d", prefix, v);
}

void ImGui::Value(const char* prefix, float v, const char* float_format)
{
	if (float_format)
	{
		char fmt[64];
		ImFormatString(fmt, IM_ARRAYSIZE(fmt), "%%s: %s", float_format);
		ImGui::Text(fmt, prefix, v);
	}
	else
		ImGui::Text("%s: %.3f", prefix, v);
}

//FIXME: May want to remove those helpers?
void ImGui::Color(const char* prefix, const ImVec4& v)
{
	ImGui::Text("%s: (%.2f,%.2f,%.2f,%.2f)", prefix, v.x, v.y, v.z, v.w);
	ImGui::SameLine();
	ImGui::ColorButton(v, true);
}

void ImGui::Color(const char* prefix, unsigned int v)
{
	ImGui::Text("%s: %08X", prefix, v);
	ImGui::SameLine();

	ImVec4 col;
	col.x = (float)((v >> 0) & 0xFF) / 255.f;
	col.y = (float)((v >> 8) & 0xFF) / 255.f;
	col.z = (float)((v >> 16) & 0xFF) / 255.f;
	col.w = (float)((v >> 24) & 0xFF) / 255.f;
	ImGui::ColorButton(col, true);
}

//-----------------------------------------------------------------------------
//PLATFORM DEPENDANT HELPERS
//-----------------------------------------------------------------------------

#if defined(_WIN32) && !defined(_WINDOWS_) && (!defined(IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCS) || !defined(IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCS))
#undef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

//Win32 API clipboard implementation
#if defined(_WIN32) && !defined(IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCS)

#ifdef _MSC_VER
#pragma comment(lib, "user32")
#endif

static const char* GetClipboardTextFnDefaultImpl()
{
	static char* buf_local = NULL;
	if (buf_local)
	{
		ImGui::MemFree(buf_local);
		buf_local = NULL;
	}
	if (!OpenClipboard(NULL))
		return NULL;
	HANDLE wbuf_handle = GetClipboardData(CF_UNICODETEXT);
	if (wbuf_handle == NULL)
		return NULL;
	if (ImWchar* wbuf_global = (ImWchar*)GlobalLock(wbuf_handle))
	{
		int buf_len = ImTextCountUtf8BytesFromStr(wbuf_global, NULL) + 1;
		buf_local = (char*)ImGui::MemAlloc(buf_len * sizeof(char));
		ImTextStrToUtf8(buf_local, buf_len, wbuf_global, NULL);
	}
	GlobalUnlock(wbuf_handle);
	CloseClipboard();
	return buf_local;
}

static void SetClipboardTextFnDefaultImpl(const char* text)
{
	if (!OpenClipboard(NULL))
		return;

	const int wbuf_length = ImTextCountCharsFromUtf8(text, NULL) + 1;
	HGLOBAL wbuf_handle = GlobalAlloc(GMEM_MOVEABLE, (SIZE_T)wbuf_length * sizeof(ImWchar));
	if (wbuf_handle == NULL)
		return;
	ImWchar* wbuf_global = (ImWchar*)GlobalLock(wbuf_handle);
	ImTextStrFromUtf8(wbuf_global, wbuf_length, text, NULL);
	GlobalUnlock(wbuf_handle);
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, wbuf_handle);
	CloseClipboard();
}

#else

//Local ImGui-only clipboard implementation, if user hasn't defined better clipboard handlers
static const char* GetClipboardTextFnDefaultImpl()
{
	return GImGui->PrivateClipboard;
}

//Local ImGui-only clipboard implementation, if user hasn't defined better clipboard handlers
static void SetClipboardTextFnDefaultImpl(const char* text)
{
	ImGuiState& g = *GImGui;
	if (g.PrivateClipboard)
	{
		ImGui::MemFree(g.PrivateClipboard);
		g.PrivateClipboard = NULL;
	}
	const char* textEnd = text + strlen(text);
	g.PrivateClipboard = (char*)ImGui::MemAlloc((size_t)(textEnd - text) + 1);
	memcpy(g.PrivateClipboard, text, (size_t)(textEnd - text));
	g.PrivateClipboard[(int)(textEnd - text)] = 0;
}

#endif

//Win32 API IME support (for Asian languages, etc.)
#if defined(_WIN32) && !defined(IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCS)

#include <imm.h>
#ifdef _MSC_VER
#pragma comment(lib, "imm32")
#endif

static void ImeSetInputScreenPosFnDefaultImpl(int x, int y)
{
	//Notify OS Input Method Editor of text input position
	if (HWND hwnd = (HWND)GImGui->IO.ImeWindowHandle)
	{
		if (HIMC himc = ImmGetContext(hwnd))
		{
			COMPOSITIONFORM cf;
			cf.ptCurrentPos.x = x;
			cf.ptCurrentPos.y = y;
			cf.dwStyle = CFS_FORCE_POSITION;
			ImmSetCompositionWindow(himc, &cf);
		}
	}
}

#else

static void ImeSetInputScreenPosFnDefaultImpl(int, int) {}

#endif

//-----------------------------------------------------------------------------
//HELP
//-----------------------------------------------------------------------------

void ImGui::ShowMetricsWindow(bool* opened)
{
	if (ImGui::Begin("ImGui Metrics", opened))
	{
		ImGui::Text("ImGui %s", ImGui::GetVersion());
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("%d vertices, %d indices (%d triangles)", ImGui::GetIO().MetricsRenderVertices, ImGui::GetIO().MetricsRenderIndices, ImGui::GetIO().MetricsRenderIndices / 3);
		ImGui::Text("%d allocations", ImGui::GetIO().MetricsAllocs);
		static bool show_clipRects = true;
		ImGui::Checkbox("Show clipping rectangles when hovering a ImDrawCmd", &show_clipRects);
		ImGui::Separator();

		struct Funcs
		{
			static void NodeDrawList(ImDrawList* drawList, const char* label)
			{
				bool node_opened = ImGui::TreeNode(drawList, "%s: '%s' %d vtx, %d indices, %d cmds", label, drawList->_OwnerName ? drawList->_OwnerName : "", drawList->VtxBuffer.Size, drawList->IdxBuffer.Size, drawList->CmdBuffer.Size);
				if (drawList == ImGui::GetWindowDrawList())
				{
					ImGui::SameLine();
					ImGui::TextColored(ImColor(255, 100, 100), "CURRENTLY APPENDING");	//Can't display stats for active draw list! (we don't have the data double-buffered)
				}
				if (!node_opened)
					return;

				int elem_offset = 0;
				for (const ImDrawCmd* pcmd = drawList->CmdBuffer.begin(); pcmd < drawList->CmdBuffer.end(); elem_offset += pcmd->ElemCount, pcmd++)
				{
					if (pcmd->UserCallback)
						ImGui::BulletText("Callback %p, userData %p", pcmd->UserCallback, pcmd->UserCallbackData);
					else
					{
						ImGui::BulletText("Draw %d indexed vtx, tex = %p, clipRect = (%.f,%.f)..(%.f,%.f)", pcmd->ElemCount, pcmd->TextureId, pcmd->ClipRect.x, pcmd->ClipRect.y, pcmd->ClipRect.z, pcmd->ClipRect.w);
						if (show_clipRects && ImGui::IsItemHovered())
						{
							ImRect clipRect = pcmd->ClipRect;
							ImRect vtxs_rect;
							for (int i = elem_offset; i < elem_offset + (int)pcmd->ElemCount; i++)
								vtxs_rect.Add(drawList->VtxBuffer[drawList->IdxBuffer[i]].pos);
							GImGui->OverlayDrawList.PushClipRectFullScreen();
							clipRect.Round();
							GImGui->OverlayDrawList.AddRect(clipRect.Min, clipRect.Max, ImColor(255, 255, 0));
							vtxs_rect.Round();
							GImGui->OverlayDrawList.AddRect(vtxs_rect.Min, vtxs_rect.Max, ImColor(255, 0, 255));
							GImGui->OverlayDrawList.PopClipRect();
						}
					}
				}
				ImGui::TreePop();
			}

			static void NodeWindows(ImVector<ImGuiWindow*>& windows, const char* label)
			{
				if (!ImGui::TreeNode(label, "%s (%d)", label, windows.Size))
					return;
				for (int i = 0; i < windows.Size; i++)
					Funcs::NodeWindow(windows[i], "Window");
				ImGui::TreePop();
			}

			static void NodeWindow(ImGuiWindow* window, const char* label)
			{
				if (!ImGui::TreeNode(window, "%s '%s', %d @ 0x%p", label, window->Name, window->Active || window->WasActive, window))
					return;
				NodeDrawList(window->DrawList, "DrawList");
				if (window->RootWindow != window) NodeWindow(window->RootWindow, "RootWindow");
				if (window->DC.ChildWindows.Size > 0) NodeWindows(window->DC.ChildWindows, "ChildWindows");
				ImGui::BulletText("Storage: %d bytes", window->StateStorage.Data.Size * (int)sizeof(ImGuiStorage::Pair));
				ImGui::TreePop();
			}
		};

		ImGuiState& g = *GImGui;	//Access private state
		g.DisableHideTextAfterDoubleHash++;	//Not exposed (yet). Disable processing that hides text after '##' markers.
		Funcs::NodeWindows(g.Windows, "Windows");
		if (ImGui::TreeNode("DrawList", "Active DrawLists (%d)", g.RenderDrawLists[0].Size))
		{
			for (int i = 0; i < g.RenderDrawLists[0].Size; i++)
				Funcs::NodeDrawList(g.RenderDrawLists[0][i], "DrawList");
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Popups", "Opened Popups Stack (%d)", g.OpenedPopupStack.Size))
		{
			for (int i = 0; i < g.OpenedPopupStack.Size; i++)
			{
				ImGuiWindow* window = g.OpenedPopupStack[i].Window;
				ImGui::BulletText("PopupID: %08x, Window: '%s'%s%s", g.OpenedPopupStack[i].PopupID, window ? window->Name : "NULL", window && (window->Flags & ImGuiWindowFlags_ChildWindow) ? " ChildWindow" : "", window && (window->Flags & ImGuiWindowFlags_ChildMenu) ? " ChildMenu" : "");
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Basic state"))
		{
			ImGui::Text("FocusedWindow: '%s'", g.FocusedWindow ? g.FocusedWindow->Name : "NULL");
			ImGui::Text("HoveredWindow: '%s'", g.HoveredWindow ? g.HoveredWindow->Name : "NULL");
			ImGui::Text("HoveredRootWindow: '%s'", g.HoveredRootWindow ? g.HoveredRootWindow->Name : "NULL");
			ImGui::Text("HoveredID: 0x%08X/0x%08X", g.HoveredId, g.HoveredIdPreviousFrame);	//Data is "in-flight" so depending on when the Metrics window is called we may see current frame information or not
			ImGui::Text("ActiveID: 0x%08X/0x%08X", g.ActiveId, g.ActiveIdPreviousFrame);
			ImGui::TreePop();
		}
		g.DisableHideTextAfterDoubleHash--;
	}
	ImGui::End();
}

//-----------------------------------------------------------------------------

//---- Include imgui_user.inl at the end of imgui.cpp
//---- So you can include code that extends ImGui using its private data/functions.
#ifdef IMGUI_INCLUDE_IMGUI_USER_INL
#include "imgui_user.inl"
#endif

//-----------------------------------------------------------------------------
