#include <windows.h>
 
#include <string>
#include <cctype>
#include <vector>
#include <commctrl.h>

#include "strings.h"
#include "Calc.h"
#include "stack_calc.h"
#include "infotable.h"




 
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
 

inline LONG make_long(int l, int r) {
	return ((LONG)(((WORD)(((DWORD_PTR)(l)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(r)) & 0xffff))) << 16));
 }
inline WPARAM make_wparam(int l, int r) {
	return ((WPARAM)(DWORD)make_long(l, r));
 }
 
LRESULT CALLBACK FilterDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	
	switch (uMsg) {
	case WM_COMMAND: {			
		if (std::unordered_map<WPARAM, int>  keys{ {VK_ADD, KEY_DISJUNCTION}, { VK_MULTIPLY,KEY_CONJUNCTION } }; keys.contains(wParam)) {
			return SendMessage(GetParent(hWnd), WM_COMMAND, make_wparam(keys[wParam], BN_CLICKED), NULL);
		}
		else break;

	}
		
	 
	case WM_CHAR: {
		if (std::unordered_map<WPARAM, int>  keys{ {'+', KEY_DISJUNCTION},  {'|', KEY_DISJUNCTION}, {'*',KEY_CONJUNCTION }, {'&',KEY_CONJUNCTION },
			{'!', KEY_NEGATION}, {'>', KEY_IMPLICATION}, {'^', KEY_XOR},
				{'=', KEY_EQUIVALENCE}, { '0', KEY_FALSE }, { '1', KEY_TRUE } }; keys.contains(wParam)) {
			return SendMessage(GetParent(hWnd), WM_COMMAND, make_wparam(keys[wParam], BN_CLICKED), NULL);
		}
		else if (iswgraph(wParam) and not iswalpha(wParam) and not (wParam == ')' or wParam == '('))
			return 0;
	}

			


	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);


 
}


void fill_table(InfoTable& lv, std::string expression) {

		truth_table_gen gen{ expression };

		auto res = gen.generate();

		for (int i{}; i < res.size(); i++) {
			if (i == 0) {
				for (int j = 0; j < res[i].size(); j++) {
					lv.add_text_column_l(std::wstring{ res[i][j].begin(), res[i][j].end() }.c_str(), j, 40);
				}
			}
			else {
				for (int j{}; j < res[i].size(); j++) {
					if (j == 0) {
						lv.insert_item(i - 1, std::wstring{ res[i][j].begin(), res[i][j].end() });
					}
					else {
						lv.insert_subitem(i - 1, j, std::wstring{ res[i][j].begin(), res[i][j].end() });
					}
				}
			}


		}
	
	 





}

BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	HWND text_edit = GetDlgItem(hWnd, IDC_INPUT);

	static InfoTable lv;
 
 
	switch (uMsg) {

		
	case WM_COMMAND: {

		int command = LOWORD(wParam);
	
		
		if (std::unordered_map<int, const wchar_t*>  commands = { {KEY_DISJUNCTION, L" ∨ "}, { KEY_CONJUNCTION, L" ∧ "},
			{KEY_NEGATION, L" ¬ "}, {KEY_IMPLICATION, L" ⇒ "}, {KEY_XOR, L" ⊕ " }, {KEY_EQUIVALENCE, L" ⇔ "},
			{KEY_O_BRACKET, L"("}, {KEY_C_BRACKET, L")"}, {KEY_TRUE, L"true"}, {KEY_FALSE, L"false"}
			}; commands.contains(wParam)) {

			SendMessage(text_edit, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(commands[command]));
		
			 
		}


 
		 
		switch (command) {

		case KEYEQ: {
			lv.clear();
 
			const int len = GetWindowTextLength(text_edit) + 1;
			 
			std::wstring buffer;
			buffer.resize(len);
		 
			GetWindowText(text_edit, buffer.data(), len);
			 
			buffer.resize(len - 1);

			 
		    fill_table(lv, process_string(buffer));
			 
 
			
		 
			
			 
			break;
		}
	

		case KEYCLR: {
			lv.clear();
			 
			SendMessage(text_edit, WM_SETTEXT, NULL, reinterpret_cast<LPARAM>(L""));
			break;
		}
		
		case IDCANCEL:
			EndDialog(hWnd, TRUE);
			break;

		}
		break;
	}
	case WM_INITDIALOG: {
		 
		HFONT hFont = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, L"Roboto Medium");
		SendMessage(text_edit, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
		SetFocus(text_edit);
		

		SetWindowSubclass(text_edit, FilterDlgProc, 0, 0);

		lv = CreateWindowExW(0l, WC_LISTVIEW, L"",
			WS_VISIBLE | WS_CLIPCHILDREN | WS_BORDER | WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
			40, 500, 340, 100,
			hWnd, (HMENU)ID_TRUTH_TABLE, GetModuleHandle(NULL), 0);
		SendMessage(lv, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);

		 
		break;
	}
		 
		

	default:
 
		return FALSE;
	}

 
	return TRUE;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{


	DialogBox(hInstance, MAKEINTRESOURCE(IDD_CALCMAINDIAL), NULL, (DLGPROC)DialogProc);
	return 0;
}

