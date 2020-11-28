#pragma once
#include <string>
#include <unordered_map>
#include <algorithm>

std::wstring substring_replace(std::wstring subject, const std::wstring& search,
	const std::wstring& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::wstring::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

std::wstring process_string(std::wstring to_process) {


	std::unordered_map<wchar_t, wchar_t>  replace_dictionary{ {L'∨', L'|', }, {L'∧', L'&' },
		{L'⇒', L'>'}, {L'⊕',  L'^'}, {L'⇔', L'='}, {L'¬', L'!'} };

	for (auto& [key, value] : replace_dictionary)
	{
		std::replace(to_process.begin(), to_process.end(), key, value);
	}
	 

	to_process = substring_replace(to_process, L"true", L"1");

	to_process = substring_replace(to_process, L"false", L"0");

	return to_process;
}