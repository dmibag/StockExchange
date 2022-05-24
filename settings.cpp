//---------------------------------------------------------------------------

#pragma hdrstop

#include "settings.h"

#pragma package(smart_init)

Settings::Settings(UnicodeString inifile)
	: ini_(std::make_unique<TIniFile>(inifile)) {
}

UnicodeString Settings::GetString(UnicodeString key) const {
	auto ret = ini_->ReadString("main", key, "");
	return ret;
}

void Settings::SetString(UnicodeString key, UnicodeString value) {
	ini_->WriteString("main", key, value);
}

TDate Settings::GetDate(UnicodeString key) const {
	auto ret = ini_->ReadDate("main", key, 0);
	return ret;
}

void Settings::SetDate(UnicodeString key, TDate value) {
	ini_->WriteDate("main", key, value);
}

bool Settings::GetBool(UnicodeString key) const {
	auto ret = ini_->ReadBool("main", key, false);
	return ret;
}

void Settings::SetBool(UnicodeString key, bool value) {
	ini_->WriteBool("main", key, value);
}

int Settings::GetInteger(UnicodeString key) const {
	auto ret = ini_->ReadInteger("main", key, 0);
	return ret;
}

void Settings::SetInteger(UnicodeString key, int value) {
	ini_->WriteInteger("main", key, value);
}
//---------------------------------------------------------------------------

