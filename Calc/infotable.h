#include "framework.h"

class text_table
{
	const static DWORD TextCol = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	int columns{};
	int items{};
	HWND handle{};

	void add_column(UINT mask, int fmt, int cx, const wchar_t* pszText, int iSubitem) {
		LVCOLUMNW col{};

		col.mask = mask;
		col.fmt = fmt;
		col.cx = cx;
		col.pszText = const_cast<wchar_t*>(pszText);
		col.iSubItem = iSubitem;
		ListView_InsertColumn(handle, col.iSubItem, &col);
	}

public:
	text_table() { }
	text_table(HWND hTable) : handle(hTable)
	{
		while (ListView_DeleteColumn(handle, 0));
		clear();
		ShowWindow(handle, SW_NORMAL);
	}
	operator HWND() { return handle; }

	enum alignment {
		left = LVCFMT_LEFT,
		right = LVCFMT_RIGHT,
		center = LVCFMT_CENTER
	};


	void append_column(const std::wstring text, int width, alignment fmt = center) {
		append_column(text.c_str(), width, fmt);
	}
	void append_column(const wchar_t* text, int width, alignment fmt = center) {
		add_column(TextCol, fmt, width, text, columns);
		columns++;

	}
	void add_text_column(const wchar_t* text, int iSubItem, int width, alignment fmt = center) {
		add_column(TextCol, fmt, width, text, iSubItem); 
	}
	void add_text_column(const std::wstring text, int iSubItem, int width, alignment fmt = center) {
		add_column(TextCol, fmt, width, text.c_str(), iSubItem);
	}

	void append_item(std::wstring text) {
		insert_item(text, items);
		items++;

	}
  
	void insert_item(std::wstring text, size_t iItem) {
		LVITEMW item{};
	 

		item.mask = LVIF_TEXT;
		item.iItem = static_cast<int>(iItem);
		item.pszText = (LPWSTR)text.c_str();
		ListView_InsertItem(handle, &item);
	}
	void insert_subitem(size_t iItem, DWORD iSubItem, std::wstring text) {

		LVITEMW subitem{};
	 
		subitem.mask = LVIF_TEXT;
		subitem.iItem = static_cast<int>(iItem);
		subitem.iSubItem = iSubItem;
		subitem.pszText = (LPWSTR)text.c_str();
		ListView_InsertItem(handle, &subitem);
		ListView_SetItemText(handle, iItem, subitem.iSubItem, subitem.pszText);
	}
	 
 
	DWORD item_count() { return ListView_GetItemCount(handle); }
	void clear() { 
		 
		ListView_DeleteAllItems(handle);
		while (Header_GetItemCount(ListView_GetHeader(this->handle))) {
			ListView_DeleteColumn(handle, 0);
		}
		 
    }
	void update() { UpdateWindow(handle); }
};

 