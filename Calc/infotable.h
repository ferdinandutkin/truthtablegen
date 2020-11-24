#include <CommCtrl.h>
#include <string>
#include <Windows.h>

class InfoTable
{
	const static DWORD TextCol = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	HWND _hTable{};
	void add_column(UINT mask, int fmt, int cx, const wchar_t* pszText, int iSubitem) {
		LVCOLUMNW col{};
		 
		col.mask = mask;
		col.fmt = fmt;
		col.cx = cx;
		col.pszText = const_cast<wchar_t*>(pszText);
		col.iSubItem = iSubitem;
		ListView_InsertColumn(_hTable, col.iSubItem, &col);
	}
	void add_text_column(const wchar_t* text, int iSubItem, int width, DWORD fmt) { add_column(TextCol, fmt, width, text, iSubItem); }
public:
	InfoTable() { }
	InfoTable(HWND hTable) : _hTable(hTable)
	{
		while (ListView_DeleteColumn(_hTable, 0));
		clear();
		ShowWindow(_hTable, SW_NORMAL);
	}
	operator HWND() { return _hTable; }
 
	void add_text_column_l(const wchar_t* text, int iSubItem, int width) { add_text_column(text, iSubItem, width, LVCFMT_LEFT); }
	void add_text_column_r(const wchar_t* text, int iSubItem, int width) { add_text_column(text, iSubItem, width, LVCFMT_RIGHT); }
	void add_text_column_c(const wchar_t* text, int iSubItem, int width) { add_text_column(text, iSubItem, width, LVCFMT_CENTER); }
	void insert_item(size_t iItem, std::wstring pszText) {
		LVITEMW item{};
	 

		item.mask = LVIF_TEXT;
		item.iItem = static_cast<int>(iItem);
		item.pszText = (LPWSTR)pszText.c_str();
		ListView_InsertItem(_hTable, &item);
	}
	void insert_subitem(size_t iItem, DWORD iSubItem, std::wstring pszText) {

		LVITEMW subitem{};
	 
		subitem.mask = LVIF_TEXT;
		subitem.iItem = static_cast<int>(iItem);
		subitem.iSubItem = iSubItem;
		subitem.pszText = (LPWSTR)pszText.c_str();
		ListView_InsertItem(_hTable, &subitem);
		ListView_SetItemText(_hTable, iItem, subitem.iSubItem, subitem.pszText);
	}
	void append_item(std::wstring pszText) {
		insert_item(item_count(), pszText);
	}
	 
 
	DWORD item_count() { return ListView_GetItemCount(_hTable); }
	void clear() { 
		 
		ListView_DeleteAllItems(_hTable);
		while (Header_GetItemCount(ListView_GetHeader(this->_hTable))) {
			ListView_DeleteColumn(_hTable, 0);
		}
		 
    }
	void update() { UpdateWindow(_hTable); }
};

 