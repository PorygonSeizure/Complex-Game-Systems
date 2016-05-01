//ImGui library v1.47 WIP
//Demo code

//Don't remove this file from your project! It is useful reference code that you can execute.
//You can call ImGui::ShowTestWindow() in your code to learn about various features of ImGui.
//Everything in this file will be stripped out by the linker if you don't call ImGui::ShowTestWindow().

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#include <ctype.h>	//toupper, isprint
#include <math.h>	//sqrtf, fabsf, fmodf, powf, cosf, sinf, floorf, ceilf
#include <stdio.h>	//vsnprintf, sscanf, printf
#include <stdlib.h>	//NULL, malloc, free, qsort, atoi

#ifdef _MSC_VER
#pragma warning (disable: 4996)	//'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#define snprintf _snprintf
#endif
#ifdef __clang__
#pragma clang diagnostic ignored "-Wdeprecated-declarations"	//warning : 'xx' is deprecated: The POSIX name for this item..
																// for strdup used in demo code (so user can copy & paste the code)
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"	//warning : cast to 'void *' from smaller integer type 'int'
#pragma clang diagnostic ignored "-Wformat-security"	//warning : warning: format string is not a string literal
#endif
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"	//warning: cast to pointer from integer of different size
#pragma GCC diagnostic ignored "-Wformat-security"	//warning : format string is not a string literal (potentially insecure)
#endif

//Play it nice with Windows users. Notepad in 2015 still doesn't display text data with Unix-style \n.
#ifdef _WIN32
#define IM_NEWLINE "\r\n"
#else
#define IM_NEWLINE "\n"
#endif

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

//-----------------------------------------------------------------------------
// DEMO CODE
//-----------------------------------------------------------------------------

#ifndef IMGUI_DISABLE_TEST_WINDOWS

using ImGui::IsItemHovered;
using ImGui::SetTooltip;
using ImGui::BulletText;
using ImGui::GetIO;
using ImGui::Begin;
using ImGui::Text;
using ImGui::Separator;
using ImGui::End;
using ImGui::PushItemWidth;
using ImGui::BeginMenu;
using ImGui::EndMenu;
using ImGui::MenuItem;
using ImGui::Spacing;
using ImGui::CollapsingHeader;
using ImGui::TextWrapped;
using ImGui::Checkbox;
using ImGui::SameLine;
using ImGui::SliderFloat;

static void ShowExampleAppConsole(bool* opened);
static void ShowExampleAppLog(bool* opened);
static void ShowExampleAppLayout(bool* opened);
static void ShowExampleAppLongText(bool* opened);
static void ShowExampleAppAutoResize(bool* opened);
static void ShowExampleAppFixedOverlay(bool* opened);
static void ShowExampleAppManipulatingWindowTitle(bool* opened);
static void ShowExampleAppCustomRendering(bool* opened);
static void ShowExampleAppMainMenuBar();
static void ShowExampleMenuFile();

static void ShowHelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");	//done
    if (IsItemHovered())
        SetTooltip(desc);
}

void ImGui::ShowUserGuide()	//done
{
    BulletText("Double-click on title bar to collapse window.");
    BulletText("Click and drag on lower right corner to resize window.");
    BulletText("Click and drag on any empty space to move window.");
    BulletText("Mouse Wheel to scroll.");
    if (GetIO().FontAllowUserScaling)
        BulletText("CTRL+Mouse Wheel to zoom window contents.");
    BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
    BulletText("CTRL+Click on a slider or drag box to input text.");
    BulletText(
        "While editing text:\n"
        "- Hold SHIFT or use mouse to select text\n"
        "- CTRL+Left/Right to word jump\n"
        "- CTRL+A or double-click to select all\n"
        "- CTRL+X,CTRL+C,CTRL+V clipboard\n"
        "- CTRL+Z,CTRL+Y undo/redo\n"
        "- ESCAPE to revert\n"
        "- You can apply arithmetic operators +,*,/ on numerical values.\n"
        "  Use +- to subtract.\n");
}

//Demonstrate most ImGui features (big function!)
void ImGui::ShowTestWindow(bool* opened)	//done
{
    //Examples apps
    static bool showAppMetrics = false;
    static bool showAppMainMenuBar = false;
    static bool showAppConsole = false;
    static bool showAppLog = false;
    static bool showAppLayout = false;
    static bool showAppLongText = false;
    static bool showAppAutoResize = false;
    static bool showAppFixedOverlay = false;
    static bool showAppCustomRendering = false;
    static bool showAppManipulatingWindowTitle = false;
    static bool showAppAbout = false;
    if (showAppMetrics)
		ImGui::ShowMetricsWindow(&showAppMetrics);	//done
    if (showAppMainMenuBar)
		ShowExampleAppMainMenuBar();
    if (showAppConsole)
		ShowExampleAppConsole(&showAppConsole);
    if (showAppLog)
		ShowExampleAppLog(&showAppLog);
    if (showAppLayout)
		ShowExampleAppLayout(&showAppLayout);
    if (showAppLongText)
		ShowExampleAppLongText(&showAppLongText);
    if (showAppAutoResize)
		ShowExampleAppAutoResize(&showAppAutoResize);
    if (showAppFixedOverlay)
		ShowExampleAppFixedOverlay(&showAppFixedOverlay);
    if (showAppManipulatingWindowTitle)
		ShowExampleAppManipulatingWindowTitle(&showAppManipulatingWindowTitle);
    if (showAppCustomRendering)
		ShowExampleAppCustomRendering(&showAppCustomRendering);
    if (showAppAbout)
    {
        Begin("About ImGui", &showAppAbout, ImGuiWindowFlags_AlwaysAutoResize);
        Text("ImGui %s", ImGui::GetVersion());	//done
        Separator();
        Text("By Omar Cornut and all github contributors.");
        Text("ImGui is licensed under the MIT License, see LICENSE for more information.");
        End();
    }

    static bool noTitlebar = false;
    static bool noBorder = true;
    static bool noResize = false;
    static bool noMove = false;
    static bool noScrollbar = false;
    static bool noCollapse = false;
    static bool noMenu = false;
    static float backgroundAlpha = 0.65f;

    //Demonstrate the various window flags. Typically you would just use the default.
    ImGuiWindowFlags windowFlags = 0;
    if (noTitlebar)
		windowFlags |= ImGuiWindowFlags_NoTitleBar;
    if (!noBorder)
		windowFlags |= ImGuiWindowFlags_ShowBorders;
    if (noResize)
		windowFlags |= ImGuiWindowFlags_NoResize;
    if (noMove)
		windowFlags |= ImGuiWindowFlags_NoMove;
    if (noScrollbar)
		windowFlags |= ImGuiWindowFlags_NoScrollbar;
    if (noCollapse)
		windowFlags |= ImGuiWindowFlags_NoCollapse;
    if (!noMenu)
		windowFlags |= ImGuiWindowFlags_MenuBar;
    if (!Begin("ImGui Demo", opened, ImVec2(550, 680), backgroundAlpha, windowFlags))
    {
        //Early out if the window is collapsed, as an optimization.
        End();
        return;
    }

    //PushItemWidth(ImGui::GetWindowWidth() * 0.65f);	//2/3 of the space for widget and 1/3 for labels
    PushItemWidth(-140);	//Right align, keep 140 pixels for labels

    Text("ImGui says hello.");

    //Menu
    if (ImGui::BeginMenuBar())	//done
    {
        if (BeginMenu("Menu"))
        {
            ShowExampleMenuFile();
            EndMenu();
        }
        if (BeginMenu("Examples"))
        {
            MenuItem("Main menu bar", NULL, &showAppMainMenuBar);
            MenuItem("Console", NULL, &showAppConsole);
            MenuItem("Log", NULL, &showAppLog);
            MenuItem("Simple layout", NULL, &showAppLayout);
            MenuItem("Long text display", NULL, &showAppLongText);
            MenuItem("Auto-resizing window", NULL, &showAppAutoResize);
            MenuItem("Simple overlay", NULL, &showAppFixedOverlay);
            MenuItem("Manipulating window title", NULL, &showAppManipulatingWindowTitle);
            MenuItem("Custom rendering", NULL, &showAppCustomRendering);
            EndMenu();
        }
        if (BeginMenu("Help"))
        {
            MenuItem("Metrics", NULL, &showAppMetrics);
            MenuItem("About ImGui", NULL, &showAppAbout);
            EndMenu();
        }
        ImGui::EndMenuBar();	//done
    }

    Spacing();
    if (CollapsingHeader("Help"))
    {
        TextWrapped("This window is being created by the ShowTestWindow() function. Please refer to the code for programming reference.\n\nUser Guide:");
        ImGui::ShowUserGuide();	//done
    }

    if (CollapsingHeader("Window options"))
    {
        Checkbox("no titlebar", &noTitlebar);
		SameLine(150);
        Checkbox("no border", &noBorder);
		SameLine(300);
        Checkbox("no resize", &noResize);
        Checkbox("no move", &noMove);
		SameLine(150);
        Checkbox("no scrollbar", &noScrollbar);
		SameLine(300);
        Checkbox("no collapse", &noCollapse);
        Checkbox("no menu", &noMenu);
        SliderFloat("bg alpha", &backgroundAlpha, 0.f, 1.f);

        if (ImGui::TreeNode("Style"))
        {
            ImGui::ShowStyleEditor();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Fonts", "Fonts (%d)", GetIO().Fonts->Fonts.Size))
        {
            TextWrapped("Tip: Load fonts with io.Fonts->AddFontFromFileTTF().");
            ImFontAtlas* atlas = GetIO().Fonts;
            if (ImGui::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
            {
                ImGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), 
													ImColor(255,255,255,128));
                ImGui::TreePop();
            }
            PushItemWidth(100);
            for (int i = 0; i < atlas->Fonts.Size; i++)
            {
                ImFont* font = atlas->Fonts[i];
                BulletText("Font %d: \'%s\', %.2f px, %d glyphs", i, font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size);
                ImGui::TreePush((void*)i);
                if (i > 0)
				{
					SameLine();
					if (ImGui::SmallButton("Set as default"))
					{
						atlas->Fonts[i] = atlas->Fonts[0];
						atlas->Fonts[0] = font;
					}
				}
                ImGui::PushFont(font);
                Text("The quick brown fox jumps over the lazy dog");
                ImGui::PopFont();
                if (ImGui::TreeNode("Details"))
                {
                    ImGui::DragFloat("font scale", &font->Scale, 0.005f, 0.3f, 2.f, "%.1f");	//scale only this font
                    Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
                    Text("Fallback character: '%c' (%d)", font->FallbackChar, font->FallbackChar);
                    for (int config = 0; config < font->ConfigDataCount; config++)
                        BulletText("Input %d: \'%s\'", config, font->ConfigData[config].Name);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            static float windowScale = 1.f;
            ImGui::DragFloat("this window scale", &windowScale, 0.005f, 0.3f, 2.f, "%.1f");	//scale only this window
            ImGui::DragFloat("global scale", &GetIO().FontGlobalScale, 0.005f, 0.3f, 2.f, "%.1f");	//scale everything
            ImGui::PopItemWidth();
            ImGui::SetWindowFontScale(windowScale);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Logging"))
        {
            TextWrapped("The logging API redirects all text output so you can easily capture the content of a window or a block. Tree nodes can be automatically expanded. You can also call ImGui::LogText() to output directly to the log without a visual output.");
            ImGui::LogButtons();
            ImGui::TreePop();
        }
    }

	if (CollapsingHeader("Widgets"))
	{
		if (ImGui::TreeNode("Tree"))
		{
			for (int i = 0; i < 5; i++)
			{
				if (ImGui::TreeNode((void*)i, "Child %d", i))
				{
					Text("blah blah");
					SameLine();
					if (ImGui::SmallButton("print"))
						printf("Child %d pressed", i);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Bullets"))
		{
			BulletText("Bullet point 1");
			BulletText("Bullet point 2\nOn multiple lines");
			ImGui::Bullet();
			Text("Bullet point 3 (two calls)");
			ImGui::Bullet();
			ImGui::SmallButton("Button");
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Colored Text"))
		{
			//Using shortcut. You can use PushStyleColor()/PopStyleColor() for more flexibility.
			ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Pink");
			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Yellow");
			ImGui::TextDisabled("Disabled");	//done
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Word Wrapping"))
		{
			// Using shortcut. You can use PushTextWrapPos()/PopTextWrapPos() for more flexibility.
			TextWrapped("This text should automatically wrap on the edge of the window. The current implementation for text wrapping follows simple rules suitable for English and possibly other languages.");
			Spacing();

			static float wrapWidth = 200.f;
			SliderFloat("Wrap width", &wrapWidth, -20, 600, "%.f");

			Text("Test paragraph 1:");
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + wrapWidth, pos.y), ImVec2(pos.x + wrapWidth + 10, pos.y + ImGui::GetTextLineHeight()),
				0xFFFF00FF);
			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrapWidth);
			Text("lazy dog. This paragraph is made to fit within %.f pixels. The quick brown fox jumps over the lazy dog.", wrapWidth);
			ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 0xFF00FFFF);
			ImGui::PopTextWrapPos();

			Text("Test paragraph 2:");
			pos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + wrapWidth, pos.y), ImVec2(pos.x + wrapWidth + 10, pos.y + ImGui::GetTextLineHeight()),
				0xFFFF00FF);
			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrapWidth);
			Text("aaaaaaaa bbbbbbbb, cccccccc,dddddddd. eeeeeeee   ffffffff. gggggggg!hhhhhhhh");
			ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 0xFF00FFFF);
			ImGui::PopTextWrapPos();

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("UTF-8 Text"))
		{
			// UTF-8 test with Japanese characters
			// (needs a suitable font, try Arial Unicode or M+ fonts http://mplus-fonts.sourceforge.jp/mplus-outline-fonts/index-en.html)
			// Most compiler appears to support UTF-8 in source code (with Visual Studio you need to save your file as 'UTF-8 without signature')
			// However for the sake for maximum portability here we are *not* including raw UTF-8 character in this source file, instead we encode the string with hexadecimal constants.
			// In your own application be reasonable and use UTF-8 in source or retrieve the data from file system!
			// Note that characters values are preserved even if the font cannot be displayed, so you can safely copy & paste garbled characters into another application.
			TextWrapped("CJK text will only appears if the font was loaded with the appropriate CJK character ranges. Call io.Font->LoadFromFileTTF() manually to load extra character ranges.");
			Text("Hiragana: \xe3\x81\x8b\xe3\x81\x8d\xe3\x81\x8f\xe3\x81\x91\xe3\x81\x93 (kakikukeko)");
			Text("Kanjis: \xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e (nihongo)");
			static char buf[32] = "\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e";
			ImGui::InputText("UTF-8 input", buf, IM_ARRAYSIZE(buf));
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Images"))
		{
			TextWrapped("Below we are displaying the font texture (which is the only texture we have access to in this demo). Use the 'ImTextureID' type as storage to pass pointers or identifier to your own texture data. Hover the texture for a zoomed view!");
			ImVec2 texScreenPos = ImGui::GetCursorScreenPos();
			float texW = (float)GetIO().Fonts->TexWidth;
			float texH = (float)GetIO().Fonts->TexHeight;
			ImTextureID texID = GetIO().Fonts->TexID;
			Text("%.fx%.f", texW, texH);
			ImGui::Image(texID, ImVec2(texW, texH), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
			if (IsItemHovered())
			{
				ImGui::BeginTooltip();
				float focusSZ = 32.f;
				float focusX = ImGui::GetMousePos().x - texScreenPos.x - focusSZ * 0.5f;
				if (focusX < 0.f)
					focusX = 0.f;
				else if (focusX > texW - focusSZ)
					focusX = texW - focusSZ;
				float focusY = ImGui::GetMousePos().y - texScreenPos.y - focusSZ * 0.5f;
				if (focusY < 0.f)
					focusY = 0.f;
				else if (focusY > texH - focusSZ)
					focusY = texH - focusSZ;
				Text("Min: (%.2f, %.2f)", focusX, focusY);
				Text("Max: (%.2f, %.2f)", focusX + focusSZ, focusY + focusSZ);
				ImVec2 uv0 = ImVec2((focusX) / texW, (focusY) / texH);
				ImVec2 uv1 = ImVec2((focusX + focusSZ) / texW, (focusY + focusSZ) / texH);
				ImGui::Image(texID, ImVec2(128, 128), uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
				ImGui::EndTooltip();
			}
			TextWrapped("And now some textured buttons..");
			static int pressedCount = 0;
			for (int i = 0; i < 8; i++)
			{
				if (i > 0)
					SameLine();
				ImGui::PushID(i);
				int framePadding = -1 + i;	//-1 = uses default padding
				if (ImGui::ImageButton(texID, ImVec2(32, 32), ImVec2(0, 0), ImVec2(32.f / texW, 32 / texH), framePadding, ImColor(0, 0, 0, 255)))
					pressedCount += 1;
				ImGui::PopID();
			}
			Text("Pressed %d times.", pressedCount);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Selectables"))
		{
			if (ImGui::TreeNode("Basic"))
			{
				static bool selected[3] = { false, true, false };
				ImGui::Selectable("1. I am selectable", &selected[0]);
				ImGui::Selectable("2. I am selectable", &selected[1]);
				Text("3. I am not selectable");
				ImGui::Selectable("4. I am selectable", &selected[2]);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Rendering more text into the same block"))
			{
				static bool selected[3] = { false, false, false };
				ImGui::Selectable("main.c", &selected[0]);
				SameLine(300);
				Text(" 2,345 bytes");
				ImGui::Selectable("Hello.cpp", &selected[1]);
				SameLine(300);
				Text("12,345 bytes");
				ImGui::Selectable("Hello.h", &selected[2]);
				SameLine(300);
				Text(" 2,345 bytes");
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Grid"))
			{
				static bool selected[16] = { true, false, false, false, false, true, false, false, false, false, true, false, false, false, false, true };
				for (int i = 0; i < 16; i++)
				{
					ImGui::PushID(i);
					if (ImGui::Selectable("Me", &selected[i], 0, ImVec2(50, 50)))
					{
						int x = i % 4;
						int y = i / 4;
						if (x > 0)
							selected[i - 1] ^= 1;
						if (x < 3)
							selected[i + 1] ^= 1;
						if (y > 0)
							selected[i - 4] ^= 1;
						if (y < 3)
							selected[i + 4] ^= 1;
					}
					if ((i % 4) < 3) SameLine();
					ImGui::PopID();
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Filtered Text Input"))
		{
			static char buf1[64] = "";
			ImGui::InputText("default", buf1, 64);
			static char buf2[64] = "";
			ImGui::InputText("decimal", buf2, 64, ImGuiInputTextFlags_CharsDecimal);
			static char buf3[64] = "";
			ImGui::InputText("hexadecimal", buf3, 64, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
			static char buf4[64] = "";
			ImGui::InputText("uppercase", buf4, 64, ImGuiInputTextFlags_CharsUppercase);
			static char buf5[64] = "";
			ImGui::InputText("no blank", buf5, 64, ImGuiInputTextFlags_CharsNoBlank);
			struct TextFilters
			{
				static int FilterImGuiLetters(ImGuiTextEditCallbackData* data)
				{
					if (data->EventChar < 256 && strchr("imgui", (char)data->EventChar))
						return 0;
					return 1;
				}
			};
			static char buf6[64] = "";
			ImGui::InputText("\"imgui\" letters", buf6, 64, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::FilterImGuiLetters);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Multi-line Text Input"))
		{
			static bool readOnly = false;
			static char text[1024 * 16] =
				"/*\n"
				" The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
				" the hexadecimal encoding of one offending instruction,\n"
				" more formally, the invalid operand with locked CMPXCHG8B\n"
				" instruction bug, is a design flaw in the majority of\n"
				" Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
				" processors (all in the P5 microarchitecture).\n"
				"*/\n\n"
				"label:\n"
				"\tlock cmpxchg8b eax\n";

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			Checkbox("Read-only", &readOnly);
			ImGui::PopStyleVar();
			ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-1.f, ImGui::GetTextLineHeight() * 16),
				ImGuiInputTextFlags_AllowTabInput | (readOnly ? ImGuiInputTextFlags_ReadOnly : 0));
			ImGui::TreePop();
		}

		static bool a = false;
		if (ImGui::Button("Button"))
		{
			printf("Clicked\n");
			a ^= 1;
		}
		if (a)
		{
			SameLine();
			Text("Thanks for clicking me!");
		}

		static bool check = true;
		Checkbox("checkbox", &check);

		static int e = 0;
		ImGui::RadioButton("radio a", &e, 0);
		SameLine();
		ImGui::RadioButton("radio b", &e, 1);
		SameLine();
		ImGui::RadioButton("radio c", &e, 2);

		//Color buttons, demonstrate using PushID() to add unique identifier in the ID stack, and changing style.
		for (int i = 0; i < 7; i++)
		{
			if (i > 0)
				SameLine();
			ImGui::PushID(i);
			ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(i / 7.f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(i / 7.f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(i / 7.f, 0.8f, 0.8f));
			ImGui::Button("Click");
			ImGui::PopStyleColor(3);
			ImGui::PopID();
		}

		Text("Hover over me");
		if (IsItemHovered())
			SetTooltip("I am a tooltip");

		SameLine();
		Text("- or me");
		if (IsItemHovered())
		{
			ImGui::BeginTooltip();
			Text("I am a fancy tooltip");
			static float arr[] = { 0.6f, 0.1f, 1.f, 0.5f, 0.92f, 0.1f, 0.2f };
			ImGui::PlotLines("Curve", arr, IM_ARRAYSIZE(arr));
			ImGui::EndTooltip();
		}

		// Testing IMGUI_ONCE_UPON_A_FRAME macro
		//for (int i = 0; i < 5; i++)
		//{
		//  IMGUI_ONCE_UPON_A_FRAME
		//  {
		//      Text("This will be displayed only once.");
		//  }
		//}

		Separator();

		ImGui::LabelText("label", "Value");

		static int item = 1;
		ImGui::Combo("combo", &item, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");	//Combo using values packed in a single constant string (for really quick combo)

		const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK" };
		static int item2 = -1;
		ImGui::Combo("combo scroll", &item2, items, IM_ARRAYSIZE(items));	//Combo using proper array. You can also pass a callback to retrieve array value, no need to create/copy an array just for that.

		{
			static char str0[128] = "Hello, world!";
			static int i0 = 123;
			static float f0 = 0.001f;
			ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0));
			SameLine();
			ShowHelpMarker("Hold SHIFT or use mouse to select text.\n" "CTRL+Left/Right to word jump.\n" "CTRL+A or double-click to select all.\n" "CTRL+X,CTRL+C,CTRL+V clipboard.\n" "CTRL+Z,CTRL+Y undo/redo.\n" "ESCAPE to revert.\n");

			ImGui::InputInt("input int", &i0);
			SameLine();
			ShowHelpMarker("You can apply arithmetic operators +,*,/ on numerical values.\n  e.g. [ 100 ], input \'*2\', result becomes [ 200 ]\nUse +- to subtract.\n");

			ImGui::InputFloat("input float", &f0, 0.01f, 1.f);

			static float vec4a[4] = { 0.1f, 0.2f, 0.3f, 0.44f };
			ImGui::InputFloat3("input float3", vec4a);
		}

		{
			static int i1 = 50;
			static int i2 = 42;
			ImGui::DragInt("drag int", &i1, 1);
			SameLine();
			ShowHelpMarker("Click and drag to edit value.\nHold SHIFT/ALT for faster/slower edit.\nDouble-click or CTRL+click to input value.");

			ImGui::DragInt("drag int 0..100", &i2, 1, 0, 100, "%.f%%");

			static float f1 = 1.f;
			static float f2 = 0.0067f;
			ImGui::DragFloat("drag float", &f1, 0.005f);
			ImGui::DragFloat("drag small float", &f2, 0.0001f, 0.f, 0.f, "%.06f ns");
		}

		{
			static int i1 = 0;
			ImGui::SliderInt("slider int", &i1, -1, 3);
			SameLine();
			ShowHelpMarker("CTRL+click to input value.");

			static float f1 = 0.123f;
			static float f2 = 0.f;
			SliderFloat("slider float", &f1, 0.f, 1.f, "ratio = %.3f");
			SliderFloat("slider log float", &f2, -10.f, 10.f, "%.4f", 3.f);
			static float angle = 0.f;
			ImGui::SliderAngle("slider angle", &angle);
		}

		static float col1[3] = { 1.f, 0.f, 0.2f };
		static float col2[4] = { 0.4f, 0.7f, 0.f, 0.5f };
		ImGui::ColorEdit3("color 1", col1);
		SameLine();
		ShowHelpMarker("Click on the colored square to change edit mode.\nCTRL+click on individual component to input value.\n");

		ImGui::ColorEdit4("color 2", col2);

		const char* listboxItems[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
		static int listboxItemCurrent = 1;
		ImGui::ListBox("listbox\n(single select)", &listboxItemCurrent, listboxItems, IM_ARRAYSIZE(listboxItems), 4);

		//static int listboxItemCurrent2 = 2;
		//PushItemWidth(-1);
		//ImGui::ListBox("##listbox2", &listboxItemCurrent2, listboxItems, IM_ARRAYSIZE(listboxItems), 4);
		//ImGui::PopItemWidth();

		if (ImGui::TreeNode("Range Widgets"))
		{
			ImGui::Unindent();

			static float begin = 10;
			static float end = 90;
			static int beginI = 100;
			static int endI = 1000;
			ImGui::DragFloatRange2("range", &begin, &end, 0.25f, 0.f, 100.f, "Min: %.1f %%", "Max: %.1f %%");
			ImGui::DragIntRange2("range int (no bounds)", &beginI, &endI, 5, 0, 0, "Min: %.f units", "Max: %.f units");

			ImGui::Indent();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Multi-component Widgets"))
		{
			ImGui::Unindent();

			static float vec4F[4] = { 0.1f, 0.2f, 0.3f, 0.44f };
			static int vec4I[4] = { 1, 5, 100, 255 };

			ImGui::InputFloat2("input float2", vec4F);
			ImGui::DragFloat2("drag float2", vec4F, 0.01f, 0.f, 1.f);
			ImGui::SliderFloat2("slider float2", vec4F, 0.f, 1.f);
			ImGui::DragInt2("drag int2", vec4I, 1, 0, 255);
			ImGui::InputInt2("input int2", vec4I);
			ImGui::SliderInt2("slider int2", vec4I, 0, 255);
			Spacing();

			ImGui::InputFloat3("input float3", vec4F);
			ImGui::DragFloat3("drag float3", vec4F, 0.01f, 0.f, 1.f);
			ImGui::SliderFloat3("slider float3", vec4F, 0.f, 1.f);
			ImGui::DragInt3("drag int3", vec4I, 1, 0, 255);
			ImGui::InputInt3("input int3", vec4I);
			ImGui::SliderInt3("slider int3", vec4I, 0, 255);
			Spacing();

			ImGui::InputFloat4("input float4", vec4F);
			ImGui::DragFloat4("drag float4", vec4F, 0.01f, 0.f, 1.f);
			ImGui::SliderFloat4("slider float4", vec4F, 0.f, 1.f);
			ImGui::InputInt4("input int4", vec4I);
			ImGui::DragInt4("drag int4", vec4I, 1, 0, 255);
			ImGui::SliderInt4("slider int4", vec4I, 0, 255);

			ImGui::Indent();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Vertical Sliders"))
		{
			ImGui::Unindent();
			const float spacing = 4;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, spacing));

			static int intVaue = 0;
			ImGui::VSliderInt("##int", ImVec2(18, 160), &intVaue, 0, 5);
			SameLine();

			static float values[7] = { 0.f, 0.6f, 0.35f, 0.9f, 0.7f, 0.2f, 0.f };
			ImGui::PushID("set1");
			for (int i = 0; i < 7; i++)
			{
				if (i > 0)
					SameLine();
				ImGui::PushID(i);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor::HSV(i / 7.f, 0.5f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor::HSV(i / 7.f, 0.6f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImColor::HSV(i / 7.f, 0.7f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImColor::HSV(i / 7.f, 0.9f, 0.9f));
				ImGui::VSliderFloat("##v", ImVec2(18, 160), &values[i], 0.f, 1.f, "");
				if (ImGui::IsItemActive() || IsItemHovered())
					SetTooltip("%.3f", values[i]);
				ImGui::PopStyleColor(4);
				ImGui::PopID();
			}
			ImGui::PopID();

			SameLine();
			ImGui::PushID("set2");
			static float values2[4] = { 0.2f, 0.8f, 0.4f, 0.25f };
			const int rows = 3;
			const ImVec2 smallSliderSize(18, (160.f - (rows - 1)*spacing) / rows);
			for (int nx = 0; nx < 4; nx++)
			{
				if (nx > 0)
					SameLine();
				ImGui::BeginGroup();
				for (int ny = 0; ny < rows; ny++)
				{
					ImGui::PushID(nx*rows + ny);
					ImGui::VSliderFloat("##v", smallSliderSize, &values2[nx], 0.f, 1.f, "");
					if (ImGui::IsItemActive() || IsItemHovered())
						SetTooltip("%.3f", values2[nx]);
					ImGui::PopID();
				}
				ImGui::EndGroup();
			}
			ImGui::PopID();

			SameLine();
			ImGui::PushID("set3");
			for (int i = 0; i < 4; i++)
			{
				if (i > 0)
					SameLine();
				ImGui::PushID(i);
				ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 40);
				ImGui::VSliderFloat("##v", ImVec2(40, 160), &values[i], 0.f, 1.f, "%.2f");
				ImGui::PopStyleVar();
				ImGui::PopID();
			}
			ImGui::PopID();
			ImGui::PopStyleVar();

			ImGui::Indent();
			ImGui::TreePop();
		}
	}

	if (CollapsingHeader("Graphs widgets"))
	{
		static float arr[] = { 0.6f, 0.1f, 1.f, 0.5f, 0.92f, 0.1f, 0.2f };
		ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));

		static bool pause;
		static ImVector<float> values;
		if (values.empty())
		{
			values.resize(90);
			memset(values.Data, 0, values.Size * sizeof(float));
		}
		static int valuesOffset = 0;
		if (!pause)
		{
			static float refreshTime = ImGui::GetTime();	//Create dummy data at fixed 60 hz rate for the demo
			for (; ImGui::GetTime() > refreshTime + 1.f / 60.f; refreshTime += 1.f / 60.f)
			{
				static float phase = 0.f;
				values[valuesOffset] = cosf(phase);
				valuesOffset = (valuesOffset + 1) % values.Size;
				phase += 0.1f * valuesOffset;
			}
		}
		ImGui::PlotLines("##Lines", values.Data, values.Size, valuesOffset, "avg 0.0", -1.f, 1.f, ImVec2(0, 80));
		SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::BeginGroup();
		Text("Lines");
		Checkbox("pause", &pause);
		ImGui::EndGroup();
		ImGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.f, 1.f, ImVec2(0, 80));

		//Use functions to generate output
		//FIXME: This is rather awkward because current plot API only pass in indices. We probably want an API passing floats and user provide sample rate/count.
		struct Funcs
		{
			static float Sin(void*, int i)
			{
				return sinf(i * 0.1f);
			}
			static float Saw(void*, int i)
			{
				return (i & 1) ? 1.f : 0.f;
			}
		};
		static int funcType = 0;
		static int displayCount = 70;
		Separator();
		PushItemWidth(100);
		ImGui::Combo("func", &funcType, "Sin\0Saw\0");
		ImGui::PopItemWidth();
		SameLine();
		ImGui::SliderInt("Sample count", &displayCount, 1, 500);
		float(*func)(void*, int) = (funcType == 0) ? Funcs::Sin : Funcs::Saw;
		ImGui::PlotLines("Lines", func, NULL, displayCount, 0, NULL, -1.f, 1.f, ImVec2(0, 80));
		ImGui::PlotHistogram("Histogram", func, NULL, displayCount, 0, NULL, -1.f, 1.f, ImVec2(0, 80));
	}

    if (CollapsingHeader("Layout"))
    {
		if (ImGui::TreeNode("Child regions"))
		{
			Text("Without border");
			static int line = 50;
			bool gotoLine = ImGui::Button("Goto");
			SameLine();
			PushItemWidth(100);
			gotoLine |= ImGui::InputInt("##Line", &line, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();
			ImGui::BeginChild("Sub1", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 300), false, ImGuiWindowFlags_HorizontalScrollbar);
			for (int i = 0; i < 100; i++)
			{
				Text("%04d: scrollable region", i);
				if (gotoLine && line == i)
					ImGui::SetScrollHere();
			}
			if (gotoLine && line >= 100)
				ImGui::SetScrollHere();
			ImGui::EndChild();

			SameLine();

			ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 5.f);
			ImGui::BeginChild("Sub2", ImVec2(0, 300), true);
			Text("With border");
			ImGui::Columns(2);
			for (int i = 0; i < 100; i++)
			{
				if (i == 50)
					ImGui::NextColumn();
				char buf[32];
				sprintf(buf, "%08x", i * 5731);
				ImGui::Button(buf, ImVec2(-1.f, 0.f));
			}
			ImGui::EndChild();
			ImGui::PopStyleVar();

			ImGui::TreePop();
		}

        if (ImGui::TreeNode("Widgets Alignment"))
        {
            static float f = 0.f;
            Text("PushItemWidth(100)");
            PushItemWidth(100);
            ImGui::DragFloat("float##1", &f);
            ImGui::PopItemWidth();

            Text("PushItemWidth(GetWindowWidth() * 0.5f);");
            PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
            ImGui::DragFloat("float##2", &f);
            ImGui::PopItemWidth();

            Text("PushItemWidth(GetContentRegionAvailWidth() * 0.5f);");
            PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.5f);
            ImGui::DragFloat("float##3", &f);
            ImGui::PopItemWidth();

            Text("PushItemWidth(-100);");
            PushItemWidth(-100);
            ImGui::DragFloat("float##4", &f);
            ImGui::PopItemWidth();

            ImGui::TreePop();
        }

		if (ImGui::TreeNode("Basic Horizontal Layout"))
		{
			TextWrapped("(Use ImGui::SameLine() to keep adding items to the right of the preceeding item)");

			//Text
			Text("Two items: Hello");
			SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Sailor");

			//Adjust spacing
			Text("More spacing: Hello");
			SameLine(0, 20);
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Sailor");

			//Button
			ImGui::AlignFirstTextHeightToWidgets();
			Text("Normal buttons");
			SameLine();
			ImGui::Button("Banana");
			SameLine();
			ImGui::Button("Apple");
			SameLine();
			ImGui::Button("Corniflower");

			//Button
			Text("Small buttons");
			SameLine();
			ImGui::SmallButton("Like this one");
			SameLine();
			Text("can fit within a text block.");

			//Aligned to arbitrary position. Easy/cheap column.
			Text("Aligned");
			SameLine(150);
			Text("x=150");
			SameLine(300);
			Text("x=300");
			Text("Aligned");
			SameLine(150);
			ImGui::SmallButton("x=150");
			SameLine(300);
			ImGui::SmallButton("x=300");

			//Checkbox
			static bool c1 = false;
			static bool c2 = false;
			static bool c3 = false;
			static bool c4 = false;
			Checkbox("My", &c1);
			SameLine();
			Checkbox("Tailor", &c2);
			SameLine();
			Checkbox("Is", &c3);
			SameLine();
			Checkbox("Rich", &c4);

			//Various
			static float f0 = 1.f;
			static float f1 = 2.f;
			static float f2 = 3.f;
			PushItemWidth(80);
			const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD" };
			static int item = -1;
			ImGui::Combo("Combo", &item, items, IM_ARRAYSIZE(items));
			SameLine();
			SliderFloat("X", &f0, 0.f, 5.f);
			SameLine();
			SliderFloat("Y", &f1, 0.f, 5.f);
			SameLine();
			SliderFloat("Z", &f2, 0.f, 5.f);
			ImGui::PopItemWidth();

			PushItemWidth(80);
			Text("Lists:");
			static int selection[4] = { 0, 1, 2, 3 };
			for (int i = 0; i < 4; i++)
			{
				if (i > 0)
					SameLine();
				ImGui::PushID(i);
				ImGui::ListBox("", &selection[i], items, IM_ARRAYSIZE(items));
				ImGui::PopID();
				//if (IsItemHovered())
				//	SetTooltip("ListBox %d hovered", i);
			}
			ImGui::PopItemWidth();

			//Dummy
			ImVec2 sz(30, 30);
			ImGui::Button("A", sz);
			SameLine();
			ImGui::Dummy(sz);
			SameLine();
			ImGui::Button("B", sz);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Groups"))
		{
			TextWrapped("(Using ImGui::BeginGroup()/EndGroup() to layout items. BeginGroup() basically locks the horizontal position. EndGroup() bundles the whole group so that you can use functions such as IsItemHovered() on it.)");
			ImGui::BeginGroup();
			{
				ImGui::BeginGroup();
				ImGui::Button("AAA");
				SameLine();
				ImGui::Button("BBB");
				SameLine();
				ImGui::BeginGroup();
				ImGui::Button("CCC");
				ImGui::Button("DDD");
				ImGui::EndGroup();
				if (IsItemHovered())
					SetTooltip("Group hovered");
				SameLine();
				ImGui::Button("EEE");
				ImGui::EndGroup();
			}
			// Capture the group size and create widgets using the same size
			ImVec2 size = ImGui::GetItemRectSize();
			const float values[5] = { 0.5f, 0.2f, 0.8f, 0.6f, 0.25f };
			ImGui::PlotHistogram("##values", values, IM_ARRAYSIZE(values), 0, NULL, 0.f, 1.f, size);

			ImGui::Button("ACTION", ImVec2((size.x - ImGui::GetStyle().ItemSpacing.x) * 0.5f, size.y));
			SameLine();
			ImGui::Button("REACTION", ImVec2((size.x - ImGui::GetStyle().ItemSpacing.x) * 0.5f, size.y));
			ImGui::EndGroup();
			SameLine();

			ImGui::Button("LEVERAGE\nBUZZWORD", size);
			SameLine();

			ImGui::ListBoxHeader("List", size);
			ImGui::Selectable("Selected", true);
			ImGui::Selectable("Not Selected", false);
			ImGui::ListBoxFooter();

			ImGui::TreePop();
		}

        if (ImGui::TreeNode("Text Baseline Alignment"))
        {
            TextWrapped("(This is testing the vertical alignment that occurs on text to keep it at the same baseline as widgets. Lines only composed of text or \"small\" widgets fit in less vertical spaces than lines with normal widgets)");

            Text("One\nTwo\nThree");
			SameLine();
            Text("Hello\nWorld");
			SameLine();
            Text("Banana");

            Text("Banana");
			SameLine();
            Text("Hello\nWorld");
			SameLine();
            Text("One\nTwo\nThree");

            ImGui::Button("HOP");
			SameLine();
            Text("Banana");
			SameLine();
            Text("Hello\nWorld");
			SameLine();
            Text("Banana");

            ImGui::Button("HOP");
			SameLine();
            Text("Hello\nWorld");
			SameLine();
            Text("Banana");

            ImGui::Button("TEST");
			SameLine();
            Text("TEST");
			SameLine();
            ImGui::SmallButton("TEST");

            ImGui::AlignFirstTextHeightToWidgets();	//If your line starts with text, call this to align it to upcoming widgets.
            Text("Text aligned to Widget");
			SameLine();
            ImGui::Button("Widget");
			SameLine();
            Text("Widget");
			SameLine();
            ImGui::SmallButton("Widget");

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Scrolling"))
        {
            TextWrapped("(Use SetScrollHere() or SetScrollFromPosY() to scroll to a given position.)");
            static bool track = true;
            static int trackLine = 50;
			static int scrollToPx = 200;
            Checkbox("Track", &track);
            SameLine(130);
			track |= ImGui::DragInt("##line", &trackLine, 0.25f, 0, 99, "Line %.f");
            bool scrollTo = ImGui::Button("Scroll To");
            SameLine(130);
			scrollTo |= ImGui::DragInt("##pos_y", &scrollToPx, 1.f, 0, 9999, "y = %.f px");
            if (scrollTo)
				track = false;

            for (int i = 0; i < 5; i++)
            {
                if (i > 0)
					SameLine();
                ImGui::BeginGroup();
                Text("%s", i == 0 ? "Top" : i == 1 ? "25%" : i == 2 ? "Center" : i == 3 ? "75%" : "Bottom");
                ImGui::BeginChild(ImGui::GetID((void*)i), ImVec2(ImGui::GetWindowWidth() * 0.17f, 200.f), true);
                if (scrollTo)
                    ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + scrollToPx, i * 0.25f);
                for (int line = 0; line < 100; line++)
                {
                    if (track && line == trackLine)
                    {
                        ImGui::TextColored(ImColor(255,255,0), "Line %d", line);
                        ImGui::SetScrollHere(i * 0.25f);	//0.f:top, 0.5f:center, 1.f:bottom
                    }
                    else
                        Text("Line %d", line);
                }
                ImGui::EndChild();
                ImGui::EndGroup();
            }
            ImGui::TreePop();
        }

		if (ImGui::TreeNode("Horizontal Scrolling"))
		{
			ImGui::Bullet();
			TextWrapped("Horizontal scrolling for a window has to be enabled explicitly via the ImGuiWindowFlags_HorizontalScrollbar flag.");
			ImGui::Bullet();
			TextWrapped("You may want to explicitly specify content width by calling SetNextWindowContentWidth() before Begin().");
			static int lines = 7;
			ImGui::SliderInt("Lines", &lines, 1, 15);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.f, 1.f));
			ImGui::BeginChild("scrolling", ImVec2(0, ImGui::GetItemsLineHeightWithSpacing() * 7 + 30), true, ImGuiWindowFlags_HorizontalScrollbar);
			for (int line = 0; line < lines; line++)
			{
				// Display random stuff
				int numButtons = 10 + ((line & 1) ? line * 9 : line * 3);
				for (int n = 0; n < numButtons; n++)
				{
					if (n > 0)
						SameLine();
					ImGui::PushID(n + line * 1000);
					char numBuf[16];
					const char* label = (!(n % 15)) ? "FizzBuzz" : (!(n % 3)) ? "Fizz" : (!(n % 5)) ? "Buzz" : (sprintf(numBuf, "%d", n), numBuf);
					float hue = n * 0.05f;
					ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue, 0.6f, 0.6f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue, 0.7f, 0.7f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue, 0.8f, 0.8f));
					ImGui::Button(label, ImVec2(40.f + sinf((float)(line + n)) * 20.f, 0.f));
					ImGui::PopStyleColor(3);
					ImGui::PopID();
				}
			}
			ImGui::EndChild();
			ImGui::PopStyleVar(2);
			float scrollXDelta = 0.f;
			ImGui::SmallButton("<<");
			if (ImGui::IsItemActive())
				scrollXDelta = -GetIO().DeltaTime * 1000.f;
			SameLine();
			Text("Scroll from code");
			SameLine();
			ImGui::SmallButton(">>");
			if (ImGui::IsItemActive())
				scrollXDelta = +GetIO().DeltaTime * 1000.f;
			if (scrollXDelta != 0.f)
			{
				ImGui::BeginChild("scrolling");	//Demonstrate a trick: you can use Begin to set yourself in the context of another window (here we are already out of your child window)
				ImGui::SetScrollX(ImGui::GetScrollX() + scrollXDelta);
				End();
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Clipping"))
		{
			static ImVec2 size(100, 100), offset(50, 20);
			TextWrapped("On a per-widget basis we are occasionally clipping text CPU-side if it won't fit in its frame. Otherwise we are doing coarser clipping + passing a scissor rectangle to the renderer. The system is designed to try minimizing both execution and CPU/GPU rendering cost.");
			ImGui::DragFloat2("size", (float*)&size, 0.5f, 0.f, 200.f, "%.f");
			ImGui::DragFloat2("offset", (float*)&offset, 0.5f, -200, 200.f, "%.f");
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImVec4 clipRect(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
			ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImColor(90, 90, 120, 255));
			ImGui::GetWindowDrawList()->AddText(ImGui::GetWindowFont(), ImGui::GetWindowFontSize() * 2.f, ImVec2(pos.x + offset.x, pos.y + offset.y), 
												ImColor(255, 255, 255, 255), "Line 1 hello\nLine 2 clip me!", NULL, 0.f, &clipRect);
			ImGui::InvisibleButton("##dummy", size);
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging())
			{
				offset.x += GetIO().MouseDelta.x;
				offset.y += GetIO().MouseDelta.y;
			}
			ImGui::TreePop();
		}
    }

    if (CollapsingHeader("Popups & Modal windows"))
    {
		if (ImGui::TreeNode("Popups"))
		{
			TextWrapped("When a popup is active, it inhibits interacting with windows that are behind the popup. Clicking outside the popup closes it.");

			static int selectedFish = -1;
			const char* names[] = { "Bream", "Haddock", "Mackerel", "Pollock", "Tilefish" };
			static bool toggles[] = { true, false, false, false, false };

			if (ImGui::Button("Select.."))
				ImGui::OpenPopup("select");
			SameLine();
			Text(selectedFish == -1 ? "<None>" : names[selectedFish]);
			if (ImGui::BeginPopup("select"))
			{
				Text("Aquarium");
				Separator();
				for (int i = 0; i < IM_ARRAYSIZE(names); i++)
				{
					if (ImGui::Selectable(names[i]))
						selectedFish = i;
				}
				ImGui::EndPopup();
			}

			if (ImGui::Button("Toggle.."))
				ImGui::OpenPopup("toggle");
			if (ImGui::BeginPopup("toggle"))
			{
				for (int i = 0; i < IM_ARRAYSIZE(names); i++)
					MenuItem(names[i], "", &toggles[i]);
				if (BeginMenu("Sub-menu"))
				{
					MenuItem("Click me");
					EndMenu();
				}

				Separator();
				Text("Tooltip here");
				if (IsItemHovered())
					SetTooltip("I am a tooltip over a popup");

				if (ImGui::Button("Stacked Popup"))
					ImGui::OpenPopup("another popup");
				if (ImGui::BeginPopup("another popup"))
				{
					for (int i = 0; i < IM_ARRAYSIZE(names); i++)
						MenuItem(names[i], "", &toggles[i]);
					if (BeginMenu("Sub-menu"))
					{
						MenuItem("Click me");
						EndMenu();
					}
					ImGui::EndPopup();
				}
				ImGui::EndPopup();
			}

			if (ImGui::Button("Popup Menu.."))
				ImGui::OpenPopup("popup from button");
			if (ImGui::BeginPopup("popup from button"))
			{
				ShowExampleMenuFile();
				ImGui::EndPopup();
			}

			TextWrapped("Below we are testing adding menu items to a regular window. It's rather unusual but should work!");
			Separator();
			MenuItem("Menu item", "CTRL+M");
			if (BeginMenu("Menu"))
			{
				ShowExampleMenuFile();
				EndMenu();
			}
			Separator();

			ImGui::TreePop();
		}

        if (ImGui::TreeNode("Context menus"))
        {
            static float value = 0.5f;
            Text("Value = %.3f (<-- right-click here)", value);
            if (ImGui::BeginPopupContextItem("item context menu"))
            {
                if (ImGui::Selectable("Set to zero"))
					value = 0.f;
                if (ImGui::Selectable("Set to PI"))
					value = 3.1415f;
                ImGui::EndPopup();
            }

            static ImVec4 color = ImColor(1.f, 0.f, 1.f, 1.f);
            ImGui::ColorButton(color);
            if (ImGui::BeginPopupContextItem("color context menu"))
            {
                Text("Edit color");
                ImGui::ColorEdit3("##edit", (float*)&color);
                if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            SameLine();
			Text("(<-- right-click here)");

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Modals"))
        {
            TextWrapped("Modal windows are like popups but the user cannot close them by clicking outside the window.");

            if (ImGui::Button("Delete.."))
                ImGui::OpenPopup("Delete?");
            if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
                Separator();

                static bool dontAskMeNextTime = false;
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
                Checkbox("Don't ask me next time", &dontAskMeNextTime);
                ImGui::PopStyleVar();

                if (ImGui::Button("OK", ImVec2(120,0)))
					ImGui::CloseCurrentPopup();
                SameLine();
                if (ImGui::Button("Cancel", ImVec2(120,0)))
					ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }

            if (ImGui::Button("Stacked modals.."))
                ImGui::OpenPopup("Stacked 1");
            if (ImGui::BeginPopupModal("Stacked 1"))
            {
                Text("Hello from Stacked The First");

                if (ImGui::Button("Another one.."))
                    ImGui::OpenPopup("Stacked 2");
                if (ImGui::BeginPopupModal("Stacked 2"))
                {
                    Text("Hello from Stacked The Second");
                    if (ImGui::Button("Close"))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }

                if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }

            ImGui::TreePop();
        }
    }

	if (CollapsingHeader("Columns"))
	{
		//Basic columns
		Text("Basic:");
		ImGui::Columns(4, "mycolumns");
		Separator();
		Text("ID");
		ImGui::NextColumn();
		Text("Name");
		ImGui::NextColumn();
		Text("Path");
		ImGui::NextColumn();
		Text("Flags");
		ImGui::NextColumn();
		Separator();
		const char* names[3] = { "One", "Two", "Three" };
		const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
		static int selected = -1;
		for (int i = 0; i < 3; i++)
		{
			char label[32];
			sprintf(label, "%04d", i);
			if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
				selected = i;
			ImGui::NextColumn();
			Text(names[i]);
			ImGui::NextColumn();
			Text(paths[i]);
			ImGui::NextColumn();
			Text("....");
			ImGui::NextColumn();
		}
		ImGui::Columns(1);

		Separator();
		Spacing();

		// Scrolling columns
		/*
		Text("Scrolling:");
		ImGui::BeginChild("##header", ImVec2(0, ImGui::GetTextLineHeightWithSpacing()+ImGui::GetStyle().ItemSpacing.y));
		ImGui::Columns(3);
		Text("ID");
		ImGui::NextColumn();
		Text("Name");
		ImGui::NextColumn();
		Text("Path");
		ImGui::NextColumn();
		ImGui::Columns(1);
		Separator();
		ImGui::EndChild();
		ImGui::BeginChild("##scrollingregion", ImVec2(0, 60));
		ImGui::Columns(3);
		for (int i = 0; i < 10; i++)
		{
			Text("%04d", i);
			ImGui::NextColumn();
			Text("Foobar");
			ImGui::NextColumn();
			Text("/path/foobar/%04d/", i);
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
		ImGui::EndChild();

		Separator();
		Spacing();
		*/

		//Create multiple items in a same cell before switching to next column
		Text("Mixed items:");
		ImGui::Columns(3, "mixed");
		Separator();

		static int e = 0;
		Text("Hello");
		ImGui::Button("Banana");
		ImGui::RadioButton("radio a", &e, 0);
		ImGui::NextColumn();

		Text("ImGui");
		ImGui::Button("Apple");
		ImGui::RadioButton("radio b", &e, 1);
		static float foo = 1.f;
		ImGui::InputFloat("red", &foo, 0.05f, 0, 3);
		Text("An extra line here.");
		ImGui::NextColumn();

		Text("Sailor");
		ImGui::Button("Corniflower");
		ImGui::RadioButton("radio c", &e, 2);
		static float bar = 1.f;
		ImGui::InputFloat("blue", &bar, 0.05f, 0, 3);
		ImGui::NextColumn();

		if (CollapsingHeader("Category A"))
			Text("Blah blah blah");
		ImGui::NextColumn();
		if (CollapsingHeader("Category B"))
			Text("Blah blah blah");
		ImGui::NextColumn();
		if (CollapsingHeader("Category C"))
			Text("Blah blah blah");
		ImGui::NextColumn();
		ImGui::Columns(1);

		Separator();
		Spacing();

		//Tree items
		Text("Tree items:");
		ImGui::Columns(2, "tree items");
		Separator();
		if (ImGui::TreeNode("Hello"))
		{
			BulletText("Sailor");
			ImGui::TreePop();
		}
		ImGui::NextColumn();
		if (ImGui::TreeNode("Bonjour"))
		{
			BulletText("Marin");
			ImGui::TreePop();
		}
		ImGui::NextColumn();
		ImGui::Columns(1);

		Separator();
		Spacing();

		//Word wrapping
		Text("Word-wrapping:");
		ImGui::Columns(2, "word-wrapping");
		Separator();
		TextWrapped("The quick brown fox jumps over the lazy dog.");
		Text("Hello Left");
		ImGui::NextColumn();
		TextWrapped("The quick brown fox jumps over the lazy dog.");
		Text("Hello Right");
		ImGui::Columns(1);

		Separator();
		Spacing();

		if (ImGui::TreeNode("Inside a tree.."))
		{
			if (ImGui::TreeNode("node 1 (with borders)"))
			{
				ImGui::Columns(4);
				for (int i = 0; i < 8; i++)
				{
					Text("%c%c%c", 'a' + i, 'a' + i, 'a' + i);
					ImGui::NextColumn();
				}
				ImGui::Columns(1);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("node 2 (without borders)"))
			{
				ImGui::Columns(4, NULL, false);
				for (int i = 0; i < 8; i++)
				{
					Text("%c%c%c", 'a' + i, 'a' + i, 'a' + i);
					ImGui::NextColumn();
				}
				ImGui::Columns(1);
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}

    if (CollapsingHeader("Filtering"))
    {
        static ImGuiTextFilter filter;
        Text("Filter usage:\n"
                    "  \"\"         display all lines\n"
                    "  \"xxx\"      display lines containing \"xxx\"\n"
                    "  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
                    "  \"-xxx\"     hide lines containing \"xxx\"");
        filter.Draw();
        const char* lines[] = { "aaa1.c", "bbb1.c", "ccc1.c", "aaa2.cpp", "bbb2.cpp", "ccc2.cpp", "abc.h", "hello, world" };
		for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
		{
			if (filter.PassFilter(lines[i]))
				BulletText("%s", lines[i]);
		}
    }

    if (CollapsingHeader("Keyboard, Mouse & Focus"))
    {
        if (ImGui::TreeNode("Tabbing"))
        {
            Text("Use TAB/SHIFT+TAB to cycle through keyboard editable fields.");
            static char buf[32] = "dummy";
            ImGui::InputText("1", buf, IM_ARRAYSIZE(buf));
            ImGui::InputText("2", buf, IM_ARRAYSIZE(buf));
            ImGui::InputText("3", buf, IM_ARRAYSIZE(buf));
            ImGui::PushAllowKeyboardFocus(false);
            ImGui::InputText("4 (tab skip)", buf, IM_ARRAYSIZE(buf));
            //SameLine();
			//ShowHelperMarker("Use ImGui::PushAllowKeyboardFocus(bool)\nto disable tabbing through certain widgets.");
            ImGui::PopAllowKeyboardFocus();
            ImGui::InputText("5", buf, IM_ARRAYSIZE(buf));
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Focus from code"))
        {
            bool focus1 = ImGui::Button("Focus on 1");
			SameLine();
            bool focus2 = ImGui::Button("Focus on 2");
			SameLine();
            bool focus3 = ImGui::Button("Focus on 3");
            int hasFocus = 0;
            static char buf[128] = "click on a button to set focus";

            if (focus1)
				ImGui::SetKeyboardFocusHere();
            ImGui::InputText("1", buf, IM_ARRAYSIZE(buf));
            if (ImGui::IsItemActive())
				hasFocus = 1;

            if (focus2)
				ImGui::SetKeyboardFocusHere();
            ImGui::InputText("2", buf, IM_ARRAYSIZE(buf));
            if (ImGui::IsItemActive())
				hasFocus = 2;

            ImGui::PushAllowKeyboardFocus(false);
            if (focus3)
				ImGui::SetKeyboardFocusHere();
            ImGui::InputText("3 (tab skip)", buf, IM_ARRAYSIZE(buf));
            if (ImGui::IsItemActive())
				hasFocus = 3;
            ImGui::PopAllowKeyboardFocus();
            if (hasFocus)
                Text("Item with focus: %d", hasFocus);
            else
                Text("Item with focus: <none>");
            TextWrapped("Cursor & selection are preserved when refocusing last used item in code.");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Dragging"))
        {
            TextWrapped("You can use ImGui::GetItemActiveDragDelta() to query for the dragged amount on any widget.");
            ImGui::Button("Drag Me");
            if (ImGui::IsItemActive())
            {
                // Draw a line between the button and the mouse cursor
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                drawList->PushClipRectFullScreen();
                drawList->AddLine(ImGui::CalcItemRectClosestPoint(GetIO().MousePos, true, -2.f), GetIO().MousePos, 
									ImColor(ImGui::GetStyle().Colors[ImGuiCol_Button]), 4.f);
                drawList->PopClipRect();
                ImVec2 valueRaw = ImGui::GetMouseDragDelta(0, 0.f);
                ImVec2 valueWithLockThreshold = ImGui::GetMouseDragDelta(0);
                ImVec2 mouseDelta = GetIO().MouseDelta;
                SameLine();
				Text("Raw (%.1f, %.1f), WithLockThresold (%.1f, %.1f), MouseDelta (%.1f, %.1f)", valueRaw.x, valueRaw.y, valueWithLockThreshold.x, 
						valueWithLockThreshold.y, mouseDelta.x, mouseDelta.y);
            }
            ImGui::TreePop();
        }

		if (ImGui::TreeNode("Keyboard & Mouse State"))
		{
			ImGuiIO& io = GetIO();

			Text("MousePos: (%g, %g)", io.MousePos.x, io.MousePos.y);
			Text("Mouse down:");
			for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
			{
				if (io.MouseDownDuration[i] >= 0.f)
				{
					SameLine();
					Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]);
				}
			}
			Text("Mouse clicked:");
			for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
			{
				if (ImGui::IsMouseClicked(i))
				{
					SameLine();
					Text("b%d", i);
				}
			}
			Text("Mouse dbl-clicked:");
			for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
			{
				if (ImGui::IsMouseDoubleClicked(i))
				{
					SameLine();
					Text("b%d", i);
				}
			}
			Text("Mouse released:");
			for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
			{
				if (ImGui::IsMouseReleased(i))
				{
					SameLine();
					Text("b%d", i);
				}
			}
			Text("MouseWheel: %.1f", io.MouseWheel);

			Text("Keys down:");
			for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
			{
				if (io.KeysDownDuration[i] >= 0.f)
				{
					SameLine();
					Text("%d (%.02f secs)", i, io.KeysDownDuration[i]);
				}
			}
			Text("Keys pressed:");
			for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
			{
				if (ImGui::IsKeyPressed(i))
				{
					SameLine();
					Text("%d", i);
				}
			}
			Text("Keys release:");
			for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
			{
				if (ImGui::IsKeyReleased(i))
				{
					SameLine();
					Text("%d", i);
				}
			}
			Text("KeyMods: %s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "");

			Text("WantCaptureMouse: %s", io.WantCaptureMouse ? "true" : "false");
			Text("WantCaptureKeyboard: %s", io.WantCaptureKeyboard ? "true" : "false");
			Text("WantTextInput: %s", io.WantTextInput ? "true" : "false");

			ImGui::Button("Hover me\nto enforce\ninputs capture");
			if (IsItemHovered())
				ImGui::CaptureKeyboardFromApp();

			ImGui::TreePop();
		}

        if (ImGui::TreeNode("Mouse cursors"))
        {
            TextWrapped("Your application can render a different mouse cursor based on what ImGui::GetMouseCursor() returns. You can also set io.MouseDrawCursor to ask ImGui to render the cursor for you in software.");
            Checkbox("io.MouseDrawCursor", &GetIO().MouseDrawCursor);
            Text("Hover to see mouse cursors:");
            for (int i = 0; i < ImGuiMouseCursor_Count_; i++)
            {
                char label[32];
                sprintf(label, "Mouse cursor %d", i);
                ImGui::Bullet();
				ImGui::Selectable(label, false);
                if (IsItemHovered())
                    ImGui::SetMouseCursor(i);
            }
            ImGui::TreePop();
        }
    }

    End();
}

void ImGui::ShowStyleEditor(ImGuiStyle* ref)
{
	ImGuiStyle& style = ImGui::GetStyle();

	const ImGuiStyle def;	//Default style
	if (ImGui::Button("Revert Style"))
		style = ref ? *ref : def;
	if (ref)
	{
		SameLine();
		if (ImGui::Button("Save Style"))
			*ref = style;
	}

	PushItemWidth(ImGui::GetWindowWidth() * 0.55f);

	if (ImGui::TreeNode("Rendering"))
	{
		Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
		Checkbox("Anti-aliased shapes", &style.AntiAliasedShapes);
		PushItemWidth(100);
		ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.1f, FLT_MAX, NULL, 2.f);
		if (style.CurveTessellationTol < 0.f)
			style.CurveTessellationTol = 0.1f;
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Sizes"))
	{
		SliderFloat("Alpha", &style.Alpha, 0.2f, 1.f, "%.2f");	//Not exposing zero here so user doesn't "lose" the UI. But application code could have a toggle to switch between zero and non-zero.
		ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.f, 20.f, "%.f");
		SliderFloat("WindowRounding", &style.WindowRounding, 0.f, 16.f, "%.f");
		SliderFloat("ChildWindowRounding", &style.ChildWindowRounding, 0.f, 16.f, "%.f");
		ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.f, 20.f, "%.f");
		SliderFloat("FrameRounding", &style.FrameRounding, 0.f, 16.f, "%.f");
		ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.f, 20.f, "%.f");
		ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.f, 20.f, "%.f");
		ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.f, 10.f, "%.f");
		SliderFloat("IndentSpacing", &style.IndentSpacing, 0.f, 30.f, "%.f");
		SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.f, 20.f, "%.f");
		SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.f, 16.f, "%.f");
		SliderFloat("GrabMinSize", &style.GrabMinSize, 1.f, 20.f, "%.f");
		SliderFloat("GrabRounding", &style.GrabRounding, 0.f, 16.f, "%.f");
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Colors"))
	{
		static int outputDest = 0;
		static bool outputOnlyModified = false;
		if (ImGui::Button("Output Colors"))
		{
			if (outputDest == 0)
				ImGui::LogToClipboard();
			else
				ImGui::LogToTTY();
			ImGui::LogText("ImGuiStyle& style = ImGui::GetStyle();" IM_NEWLINE);
			for (int i = 0; i < ImGuiCol_COUNT; i++)
			{
				const ImVec4& col = style.Colors[i];
				const char* name = ImGui::GetStyleColName(i);
				if (!outputOnlyModified || memcmp(&col, (ref ? &ref->Colors[i] : &def.Colors[i]), sizeof(ImVec4)) != 0)
					ImGui::LogText("style.Colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE, name, 22 - (int)strlen(name), "", col.x, col.y, 
									col.z, col.w);
			}
			ImGui::LogFinish();
		}
		SameLine();
		PushItemWidth(120);
		ImGui::Combo("##output_type", &outputDest, "To Clipboard\0To TTY");
		ImGui::PopItemWidth();
		SameLine();
		Checkbox("Only Modified Fields", &outputOnlyModified);

		static ImGuiColorEditMode editMode = ImGuiColorEditMode_RGB;
		ImGui::RadioButton("RGB", &editMode, ImGuiColorEditMode_RGB);
		SameLine();
		ImGui::RadioButton("HSV", &editMode, ImGuiColorEditMode_HSV);
		SameLine();
		ImGui::RadioButton("HEX", &editMode, ImGuiColorEditMode_HEX);
		//Text("Tip: Click on colored square to change edit mode.");

		static ImGuiTextFilter filter;
		filter.Draw("Filter colors", 200);

		ImGui::BeginChild("#colors", ImVec2(0, 300), true);
		PushItemWidth(-160);
		ImGui::ColorEditMode(editMode);
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			const char* name = ImGui::GetStyleColName(i);
			if (!filter.PassFilter(name))
				continue;
			ImGui::PushID(i);
			ImGui::ColorEdit4(name, (float*)&style.Colors[i], true);
			if (memcmp(&style.Colors[i], (ref ? &ref->Colors[i] : &def.Colors[i]), sizeof(ImVec4)) != 0)
			{
				SameLine();
				if (ImGui::Button("Revert"))
					style.Colors[i] = ref ? ref->Colors[i] : def.Colors[i];
				if (ref)
				{
					SameLine();
					if (ImGui::Button("Save"))
						ref->Colors[i] = style.Colors[i];
				}
			}
			ImGui::PopID();
		}
		ImGui::PopItemWidth();
		ImGui::EndChild();

		ImGui::TreePop();
	}

	ImGui::PopItemWidth();
}

static void ShowExampleAppMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (BeginMenu("File"))
        {
            ShowExampleMenuFile();
            EndMenu();
        }
        if (BeginMenu("Edit"))
        {
			if (MenuItem("Undo", "CTRL+Z")) {}
            if (MenuItem("Redo", "CTRL+Y", false, false)) {}	//Disabled item
            Separator();
            if (MenuItem("Cut", "CTRL+X")) {}
            if (MenuItem("Copy", "CTRL+C")) {}
            if (MenuItem("Paste", "CTRL+V")) {}
            EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

static void ShowExampleMenuFile()
{
    MenuItem("(dummy menu)", NULL, false, false);
    if (MenuItem("New")) {}
    if (MenuItem("Open", "Ctrl+O")) {}
    if (BeginMenu("Open Recent"))
    {
        MenuItem("fish_hat.c");
        MenuItem("fish_hat.inl");
        MenuItem("fish_hat.h");
        if (BeginMenu("More.."))
        {
            MenuItem("Hello");
            MenuItem("Sailor");
            if (BeginMenu("Recurse.."))
            {
                ShowExampleMenuFile();
                EndMenu();
            }
            EndMenu();
        }
        EndMenu();
    }
    if (MenuItem("Save", "Ctrl+S")) {}
    if (MenuItem("Save As..")) {}
    Separator();
    if (BeginMenu("Options"))
    {
        static bool enabled = true;
        MenuItem("Enabled", "", &enabled);
        ImGui::BeginChild("child", ImVec2(0, 60), true);
        for (int i = 0; i < 10; i++)
            Text("Scrolling Text %d", i);
        ImGui::EndChild();
        static float f = 0.5f;
        static int n = 0;
        SliderFloat("Value", &f, 0.f, 1.f);
        ImGui::InputFloat("Input", &f, 0.1f);
        ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        EndMenu();
    }
    if (BeginMenu("Colors"))
    {
        for (int i = 0; i < ImGuiCol_COUNT; i++)
            MenuItem(ImGui::GetStyleColName((ImGuiCol)i));
        EndMenu();
    }
    if (BeginMenu("Disabled", false))	//Disabled
        IM_ASSERT(0);
    if (MenuItem("Checked", NULL, true)) {}
    if (MenuItem("Quit", "Alt+F4")) {}
}

static void ShowExampleAppAutoResize(bool* opened)
{
	if (!Begin("Example: Auto-resizing window", opened, ImGuiWindowFlags_AlwaysAutoResize))
	{
		End();
		return;
	}

	static int lines = 10;
	Text("Window will resize every-frame to the size of its content.\nNote that you probably don't want to query the window size to\noutput your content because that would create a feedback loop.");
	ImGui::SliderInt("Number of lines", &lines, 1, 20);
	for (int i = 0; i < lines; i++)
		Text("%*sThis is line %d", i * 4, "", i);	//Pad with space to extend size horizontally
	End();
}

static void ShowExampleAppFixedOverlay(bool* opened)
{
	ImGui::SetNextWindowPos(ImVec2(10, 10));
	if (!Begin("Example: Fixed Overlay", opened, ImVec2(0, 0), 0.3f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
						ImGuiWindowFlags_NoSavedSettings))
	{
		End();
		return;
	}
	Text("Simple overlay\non the top-left side of the screen.");
	Separator();
	Text("Mouse Position: (%.1f,%.1f)", GetIO().MousePos.x, GetIO().MousePos.y);
	End();
}

static void ShowExampleAppManipulatingWindowTitle(bool* opened)
{
	(void)opened;

	//By default, Windows are uniquely identified by their title.
	//You can use the "##" and "###" markers to manipulate the display/ID. Read FAQ at the top of this file!

	//Using "##" to display same title but have unique identifier.
	ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiSetCond_FirstUseEver);
	Begin("Same title as another window##1");
	Text("This is window 1.\nMy title is the same as window 2, but my identifier is unique.");
	End();

	ImGui::SetNextWindowPos(ImVec2(100, 200), ImGuiSetCond_FirstUseEver);
	Begin("Same title as another window##2");
	Text("This is window 2.\nMy title is the same as window 1, but my identifier is unique.");
	End();

	//Using "###" to display a changing title but keep a static identifier "AnimatedTitle"
	char buf[128];
	sprintf(buf, "Animated title %c %d###AnimatedTitle", "|/-\\"[(int)(ImGui::GetTime() / 0.25f) & 3], rand());
	ImGui::SetNextWindowPos(ImVec2(100, 300), ImGuiSetCond_FirstUseEver);
	Begin(buf);
	Text("This window has a changing title.");
	End();
}

static void ShowExampleAppCustomRendering(bool* opened)
{
	ImGui::SetNextWindowSize(ImVec2(300, 350), ImGuiSetCond_FirstUseEver);
	if (!Begin("Example: Custom rendering", opened))
	{
		End();
		return;
	}

	//Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of overloaded operators, etc.
	//Define IM_VEC2_CLASS_EXTRA in imconfig.h to create implicit conversions between your types and ImVec2/ImVec4.
	//ImGui defines overloaded operators but they are internal to imgui.cpp and not exposed outside (to avoid messing with your types)
	//In this example we aren't using the operators.

	static ImVector<ImVec2> points;
	static bool addingLine = false;
	if (ImGui::Button("Clear"))
		points.clear();
	if (points.Size >= 2)
	{
		SameLine();
		if (ImGui::Button("Undo"))
		{
			points.pop_back();
			points.pop_back();
		}
	}
	Text("Left-click and drag to add lines");
	Text("Right-click to undo");

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	//Here we are using InvisibleButton() as a convenience to 1) advance the cursor and 2) allows us to use IsItemHovered()
	//However you can draw directly and poll mouse/keyboard by yourself. You can manipulate the cursor using GetCursorPos() and SetCursorPos().
	//If you only use the ImDrawList API, you can notify the owner window of its extends by using SetCursorPos(max).
	ImVec2 canvasPos = ImGui::GetCursorScreenPos();	//ImDrawList API uses screen coordinates!
	ImVec2 canvasSize = ImGui::GetContentRegionAvail();	//Resize canvas to what's available
	if (canvasSize.x < 50.f)
		canvasSize.x = 50.f;
	if (canvasSize.y < 50.f)
		canvasSize.y = 50.f;
	drawList->AddRectFilledMultiColor(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), ImColor(0, 0, 0), ImColor(255, 0, 0), 
										ImColor(255, 255, 0), ImColor(0, 255, 0));
	drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), ImColor(255, 255, 255));

	drawList->AddBezierCurve(ImVec2(canvasPos.x + 20, canvasPos.y + 20), ImVec2(canvasPos.x + 100, canvasPos.y + 20), 
								ImVec2(canvasPos.x + canvasSize.x - 100, canvasPos.y + canvasSize.y - 20), 
								ImVec2(canvasPos.x + canvasSize.x - 20, canvasPos.y + canvasSize.y - 20), ImColor(255, 200, 0), 5.f);

	bool addingPreview = false;
	ImGui::InvisibleButton("canvas", canvasSize);
	if (IsItemHovered())
	{
		ImVec2 mousePosInCanvas = ImVec2(GetIO().MousePos.x - canvasPos.x, GetIO().MousePos.y - canvasPos.y);
		if (!addingLine && ImGui::IsMouseClicked(0))
		{
			points.push_back(mousePosInCanvas);
			addingLine = true;
		}
		if (addingLine)
		{
			addingPreview = true;
			points.push_back(mousePosInCanvas);
			if (!GetIO().MouseDown[0])
				addingLine = addingPreview = false;
		}
		if (ImGui::IsMouseClicked(1) && !points.empty())
		{
			addingLine = addingPreview = false;
			points.pop_back();
			points.pop_back();
		}
	}
	drawList->PushClipRect(ImVec4(canvasPos.x, canvasPos.y, canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y));	//clip lines within the canvas (if we resize it, etc.)
	for (int i = 0; i < points.Size - 1; i += 2)
		drawList->AddLine(ImVec2(canvasPos.x + points[i].x, canvasPos.y + points[i].y), ImVec2(canvasPos.x + points[i + 1].x, canvasPos.y + points[i + 1].y), 
							0xFF00FFFF, 2.f);
	drawList->PopClipRect();
	if (addingPreview)
		points.pop_back();
	End();
}

//For the console example, here we are using a more C++ like approach of declaring a class to hold the data and the functions.
struct ExampleAppConsole
{
    char inputBuf[256];
    ImVector<char*> items;
    bool scrollToBottom;
    ImVector<char*> history;
    int historyPos;	//-1: new line, 0..history.Size-1 browsing history.
    ImVector<const char*> commands;

    ExampleAppConsole()
    {
        ClearLog();
        historyPos = -1;
        commands.push_back("HELP");
        commands.push_back("HISTORY");
        commands.push_back("CLEAR");
        commands.push_back("CLASSIFY");	//"classify" is here to provide an example of "C"+[tab] completing to "CL" and displaying matches.
    }
    ~ExampleAppConsole()
    {
        ClearLog();
        for (int i = 0; i < items.Size; i++)
            free(history[i]);
    }

    void ClearLog()
    {
        for (int i = 0; i < items.Size; i++)
			free(items[i]);
        items.clear();
        scrollToBottom = true;
    }

	void AddLog(const char* fmt, ...) IM_PRINTFARGS(2)
	{
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
		buf[IM_ARRAYSIZE(buf) - 1] = 0;
		va_end(args);
		items.push_back(strdup(buf));
		scrollToBottom = true;
	}

	void Draw(const char* title, bool* opened)
	{
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiSetCond_FirstUseEver);
		if (!Begin(title, opened))
		{
			End();
			return;
		}

		TextWrapped("This example implements a console with basic coloring, completion and history. A more elaborate implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
		TextWrapped("Enter 'HELP' for help, press TAB to use text completion.");

		//TODO: display items starting from the bottom

		if (ImGui::SmallButton("Add Dummy Text"))
		{
			AddLog("%d some text", items.Size);
			AddLog("some more text");
			AddLog("display very important message here!");
		}
		SameLine();
		if (ImGui::SmallButton("Add Dummy Error"))
			AddLog("[error] something went wrong");
		SameLine();
		if (ImGui::SmallButton("Clear"))
			ClearLog();
		//static float t = 0.f;
		//if (ImGui::GetTime() - t > 0.02f)
		//{
		//	t = ImGui::GetTime();
		//	AddLog("Spam %f", t);
		//}

		Separator();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		static ImGuiTextFilter filter;
		filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
		ImGui::PopStyleVar();
		Separator();

		//Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
		//NB- if you have thousands of entries this approach may be too inefficient. You can seek and display only the lines that are visible - CalcListClipping() is a helper to compute this information.
		//If your items are of variable size you may want to implement code similar to what CalcListClipping() does. Or split your data into fixed height items to allow random-seeking into your list.
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Clear"))
				ClearLog();
			ImGui::EndPopup();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));	//Tighten spacing
		for (int i = 0; i < items.Size; i++)
		{
			const char* item = items[i];
			if (!filter.PassFilter(item))
				continue;
			ImVec4 col = ImColor(255, 255, 255);	//A better implementation may store a type per-item. For the sample let's just parse the text.
			if (strstr(item, "[error]"))
				col = ImColor(255, 100, 100);
			else if (strncmp(item, "# ", 2) == 0)
				col = ImColor(255, 200, 150);
			ImGui::PushStyleColor(ImGuiCol_Text, col);
			ImGui::TextUnformatted(item);
			ImGui::PopStyleColor();
		}
		if (scrollToBottom)
			ImGui::SetScrollHere();
		scrollToBottom = false;
		ImGui::PopStyleVar();
		ImGui::EndChild();
		Separator();

		//Command-line
		if (ImGui::InputText("Input", inputBuf, IM_ARRAYSIZE(inputBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | 
								ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
		{
			char* inputEnd = inputBuf + strlen(inputBuf);
			while (inputEnd > inputBuf && inputEnd[-1] == ' ')
				inputEnd--;
			*inputEnd = 0;
			if (inputBuf[0])
				ExecCommand(inputBuf);
			strcpy(inputBuf, "");
		}

		//Demonstrate keeping auto focus on the input box
		if (IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
			ImGui::SetKeyboardFocusHere(-1);	//Auto focus

		End();
	}

    static int Stricmp(const char* str1, const char* str2)
	{
		int d;
		while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1)
		{
			str1++;
			str2++;
		}
		return d;
	}
    static int Strnicmp(const char* str1, const char* str2, int count)
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

	void ExecCommand(const char* commandLine)
	{
		AddLog("# %s\n", commandLine);

		//Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
		historyPos = -1;
		for (int i = history.Size - 1; i >= 0; i--)
		{
			if (Stricmp(history[i], commandLine) == 0)
			{
				free(history[i]);
				history.erase(history.begin() + i);
				break;
			}
		}
		history.push_back(strdup(commandLine));

		//Process command
		if (Stricmp(commandLine, "CLEAR") == 0)
			ClearLog();
		else if (Stricmp(commandLine, "HELP") == 0)
		{
			AddLog("Commands:");
			for (int i = 0; i < commands.Size; i++)
				AddLog("- %s", commands[i]);
		}
		else if (Stricmp(commandLine, "HISTORY") == 0)
		{
			for (int i = history.Size >= 10 ? history.Size - 10 : 0; i < history.Size; i++)
				AddLog("%3d: %s\n", i, history[i]);
		}
		else
			AddLog("Unknown command: '%s'\n", commandLine);
	}

    static int TextEditCallbackStub(ImGuiTextEditCallbackData* data)	//In C++11 you are better off using lambdas for this sort of forwarding callbacks
    {
        ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
        return console->TextEditCallback(data);
    }

	int TextEditCallback(ImGuiTextEditCallbackData* data)
	{
		//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
		switch (data->EventFlag)
		{
		case ImGuiInputTextFlags_CallbackCompletion:
		{
			//Example of TEXT COMPLETION

			//Locate beginning of current word
			const char* wordEnd = data->Buf + data->CursorPos;
			const char* wordStart = wordEnd;
			while (wordStart > data->Buf)
			{
				const char c = wordStart[-1];
				if (c == ' ' || c == '\t' || c == ',' || c == ';')
					break;
				wordStart--;
			}

			//Build a list of candidates
			ImVector<const char*> candidates;
			for (int i = 0; i < commands.Size; i++)
			{
				if (Strnicmp(commands[i], wordStart, (int)(wordEnd - wordStart)) == 0)
					candidates.push_back(commands[i]);
			}

			if (candidates.Size == 0)
			{
				//No match
				AddLog("No match for \"%.*s\"!\n", (int)(wordEnd - wordStart), wordStart);
			}
			else if (candidates.Size == 1)
			{
				//Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
				data->DeleteChars((int)(wordStart - data->Buf), (int)(wordEnd - wordStart));
				data->InsertChars(data->CursorPos, candidates[0]);
				data->InsertChars(data->CursorPos, " ");
			}
			else
			{
				//Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
				int matchLen = (int)(wordEnd - wordStart);
				for (;;)
				{
					int c = 0;
					bool allCandidatesMatches = true;
					for (int i = 0; i < candidates.Size && allCandidatesMatches; i++)
					{
						if (i == 0)
							c = toupper(candidates[i][matchLen]);
						else if (c != toupper(candidates[i][matchLen]))
							allCandidatesMatches = false;
					}
					if (!allCandidatesMatches)
						break;
					matchLen++;
				}

				if (matchLen > 0)
				{
					data->DeleteChars((int)(wordStart - data->Buf), (int)(wordEnd - wordStart));
					data->InsertChars(data->CursorPos, candidates[0], candidates[0] + matchLen);
				}

				//List matches
				AddLog("Possible matches:\n");
				for (int i = 0; i < candidates.Size; i++)
					AddLog("- %s\n", candidates[i]);
			}

			break;
		}
		case ImGuiInputTextFlags_CallbackHistory:
		{
			//Example of HISTORY
			const int prevHistoryPos = historyPos;
			if (data->EventKey == ImGuiKey_UpArrow)
			{
				if (historyPos == -1)
					historyPos = history.Size - 1;
				else if (historyPos > 0)
					historyPos--;
			}
			else if (data->EventKey == ImGuiKey_DownArrow)
			{
				if (historyPos != -1)
				{
					if (++historyPos >= history.Size)
						historyPos = -1;
				}
			}

			//A better implementation would preserve the data on the current input line along with cursor position.
			if (prevHistoryPos != historyPos)
			{
				snprintf(data->Buf, data->BufSize, "%s", (historyPos >= 0) ? history[historyPos] : "");
				data->BufDirty = true;
				data->CursorPos = data->SelectionStart = data->SelectionEnd = (int)strlen(data->Buf);
			}
		}
		}
		return 0;
	}
};

static void ShowExampleAppConsole(bool* opened)
{
    static ExampleAppConsole console;
    console.Draw("Example: Console", opened);
}

//Usage:
//	static ExampleAppLog myLog;
//	myLog.AddLog("Hello %d world\n", 123);
//	myLog.Draw("title");
struct ExampleAppLog
{
    ImGuiTextBuffer buf;
    ImGuiTextFilter filter;
    ImVector<int> lineOffsets;	//Index to lines offset
    bool scrollToBottom;

    void Clear()
	{
		buf.clear();
		lineOffsets.clear();
	}

    void AddLog(const char* fmt, ...) IM_PRINTFARGS(2)
    {
        int oldSize = buf.size();
        va_list args;
        va_start(args, fmt);
        buf.appendv(fmt, args);
        va_end(args);
		for (int newSize = buf.size(); oldSize < newSize; oldSize++)
		{
			if (buf[oldSize] == '\n')
				lineOffsets.push_back(oldSize);
		}
        scrollToBottom = true;
    }

	void Draw(const char* title, bool* pOpened = NULL)
	{
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiSetCond_FirstUseEver);
		Begin(title, pOpened);
		if (ImGui::Button("Clear"))
			Clear();
		SameLine();
		bool copy = ImGui::Button("Copy");
		SameLine();
		filter.Draw("Filter", -100.f);
		Separator();
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (copy)
			ImGui::LogToClipboard();

		if (filter.IsActive())
		{
			const char* bufBegin = buf.begin();
			const char* line = bufBegin;
			for (int lineNo = 0; line != NULL; lineNo++)
			{
				const char* lineEnd = (lineNo < lineOffsets.Size) ? bufBegin + lineOffsets[lineNo] : NULL;
				if (filter.PassFilter(line, lineEnd))
					ImGui::TextUnformatted(line, lineEnd);
				line = lineEnd && lineEnd[1] ? lineEnd + 1 : NULL;
			}
		}
		else
			ImGui::TextUnformatted(buf.begin());

		if (scrollToBottom)
			ImGui::SetScrollHere(1.f);
		scrollToBottom = false;
		ImGui::EndChild();
		End();
	}
};

static void ShowExampleAppLog(bool* opened)
{
	static ExampleAppLog log;

	//Demo fill
	static float lastTime = -1.f;
	float time = ImGui::GetTime();
	if (time - lastTime >= 0.3f)
	{
		const char* randomWords[] = { "system", "info", "warning", "error", "fatal", "notice", "log" };
		log.AddLog("[%s] Hello, time is %.1f, rand() %d\n", randomWords[rand() % IM_ARRAYSIZE(randomWords)], time, (int)rand());
		lastTime = time;
	}

	log.Draw("Example: Log", opened);
}

static void ShowExampleAppLayout(bool* opened)
{
	ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiSetCond_FirstUseEver);
	if (Begin("Example: Layout", opened, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())	//done
		{
			if (BeginMenu("File"))
			{
				if (MenuItem("Close"))
					*opened = false;
				EndMenu();
			}
			ImGui::EndMenuBar();	//done
		}

		//left
		static int selected = 0;
		ImGui::BeginChild("left pane", ImVec2(150, 0), true);
		for (int i = 0; i < 100; i++)
		{
			char label[128];
			sprintf(label, "MyObject %d", i);
			if (ImGui::Selectable(label, selected == i))
				selected = i;
		}
		ImGui::EndChild();
		SameLine();

		//right
		ImGui::BeginGroup();
		ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()));	//Leave room for 1 line below us
		Text("MyObject: %d", selected);
		Separator();
		TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
		ImGui::EndChild();
		ImGui::BeginChild("buttons");
		if (ImGui::Button("Revert")) {}
		SameLine();
		if (ImGui::Button("Save")) {}
		ImGui::EndChild();
		ImGui::EndGroup();
	}
	End();
}

static void ShowExampleAppLongText(bool* opened)
{
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiSetCond_FirstUseEver);
	if (!Begin("Example: Long text display", opened))
	{
		End();
		return;
	}

	static int testType = 0;
	static ImGuiTextBuffer log;
	static int lines = 0;
	Text("Printing unusually long amount of text.");
	ImGui::Combo("Test type", &testType, "Single call to TextUnformatted()\0Multiple calls to Text(), clipped manually\0Multiple calls to Text(), not clipped\0");
	Text("Buffer contents: %d lines, %d bytes", lines, log.size());
	if (ImGui::Button("Clear"))
	{
		log.clear();
		lines = 0;
	}
	SameLine();
	if (ImGui::Button("Add 1000 lines"))
	{
		for (int i = 0; i < 1000; i++)
			log.append("%i The quick brown fox jumps over the lazy dog\n", lines + i);
		lines += 1000;
	}
	ImGui::BeginChild("Log");
	switch (testType)
	{
	case 0:
		//Single call to TextUnformatted() with a big buffer
		ImGui::TextUnformatted(log.begin(), log.end());
		break;
	case 1:
	{
		//Multiple calls to Text(), manually coarsely clipped - demonstrate how to use the ImGuiListClipper helper.
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGuiListClipper clipper(lines, ImGui::GetTextLineHeight());
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			Text("%i The quick brown fox jumps over the lazy dog\n", i);
		clipper.End();
		ImGui::PopStyleVar();
		break;
	}
	case 2:
		//Multiple calls to Text(), not clipped (slow)
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		for (int i = 0; i < lines; i++)
			Text("%i The quick brown fox jumps over the lazy dog\n", i);
		ImGui::PopStyleVar();
		break;
	}
	ImGui::EndChild();
	End();
}

//End of Demo code
#else

void ImGui::ShowTestWindow(bool*) {}
void ImGui::ShowUserGuide(bool*) {}
void ImGui::ShowStyleEditor(bool*) {}

#endif
