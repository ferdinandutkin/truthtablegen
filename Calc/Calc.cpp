#include "framework.h"
#include "strings.h"
#include "Calc.h"

#include "stack_calc.h"
#include "infotable.h"


bool enable_scrolling;//АААААААААААААААААААААААА

 
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


void fill_table(text_table& table, text_table& column, std::wstring expression) {

		truth_table_gen gen{ expression };

		auto res = gen.generate();

		column.append_column(L"", 40);
		for (int i{}; i < res.size() - 1; i++)
			column.insert_item(std::to_wstring(i), i);


		for (int i{}; i < res.size(); i++) {
			if (i == 0) {
				for (auto& column : res[i]) {
					table.append_column( column, 40);
				}
			}
			else {
				for (int j{}; j < res[i].size(); j++) {
					if (j == 0) {
						table.append_item(res[i][j]);
					}
					else {
						table.insert_subitem(i - 1, j, res[i][j]);
					}
				}
			}


		}
	
	 
		enable_scrolling = true;





}

BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	HWND text_edit = GetDlgItem(hWnd, IDC_INPUT);

	static text_table table;
	static text_table column;
	 
 
 
	switch (uMsg) {
/*

	case WM_NOTIFY: {

	

		switch (((LPNMHDR)lParam)->code)
		{
			auto SetTopIndex = [](HWND listview, long index) {
				RECT rc;

				SendMessage(listview, LVM_GETITEMRECT, 0, reinterpret_cast<LPARAM>(&rc));
				SendMessage(listview, LVM_SCROLL, 0, (index - ListView_GetTopIndex(listview)) * (rc.bottom - rc.top)); };

		case LVN_ENDSCROLL:
			if (wParam == ID_TRUTH_TABLE) {
				 
			//	MessageBox(NULL, L"", L"", MB_OK);
				SetTopIndex(column, ListView_GetTopIndex(table));
			}
			 
			break;
		}
		break;

	

		 

	}
	*/
		
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
			table.clear();
			column.clear();
 
			const int len = GetWindowTextLength(text_edit) + 1;

			if (not (len - 1))
				return TRUE;
			 
			std::wstring buffer;
		
			buffer.resize(len);
		 
			GetWindowText(text_edit, buffer.data(), len);
			 

			SetFocus(text_edit);
			 
			buffer.resize(len - 1);

			 
		    fill_table(table, column, process_string(buffer));
			 
 
			
		 
			
			 
			break;
		}
	

		case KEYCLR: {
			table.clear();
			 
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

		table = CreateWindowExW(0l, WC_LISTVIEW, L"",
			WS_VISIBLE | WS_CLIPCHILDREN | WS_BORDER | WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
			60, 380, 350, 250,
			hWnd, (HMENU)ID_TRUTH_TABLE, GetModuleHandle(NULL), 0);

		column = CreateWindowExW(0l, WC_LISTVIEW, L"",
			WS_VISIBLE | WS_CLIPCHILDREN  | WS_CHILD   | LVS_REPORT | LVS_EDITLABELS,
			20, 380, 40, 250,
			hWnd, (HMENU)ID_TRUTH_TABLE_COLUMN, GetModuleHandle(NULL), 0);


		SetProp(table, L"column", column.operator HWND());

		SetWindowSubclass(table, [](HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) -> LRESULT {
			HWND column = (HWND)GetProp(wnd, L"column");
			if (enable_scrolling) {
				SendMessageW(column, msg, wParam, lParam);
			 }
			 
 
			return DefSubclassProc(wnd, msg, wParam, lParam);
			}, 0, 0);

		SendMessage(table, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
		SendMessage(column,WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
		ShowScrollBar(column, SB_HORZ, FALSE);

		 
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

