//---------------------------------------------------------------------------

#ifndef settingsH
#define settingsH

#include <IniFiles.hpp>
#include <memory>
#include <string>

class Settings {
public:
	Settings(UnicodeString inifile);

	UnicodeString GetString(UnicodeString key) const;
	void SetString(UnicodeString key, UnicodeString value);
	TDate GetDate(UnicodeString key) const;
	void SetDate(UnicodeString key, TDate value);
	bool GetBool(UnicodeString key) const;
	void SetBool(UnicodeString key, bool value);
	int GetInteger(UnicodeString key) const;
	void SetInteger(UnicodeString key, int value);

private:
	std::unique_ptr<TIniFile> ini_;
};

//---------------------------------------------------------------------------
#endif
