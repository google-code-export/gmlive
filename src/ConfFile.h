/***************************************************************************
 *   Copyright (C) 2005 by xihe						   *
 *   xihels@163.com							   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef _CONF_FILE_
#define _CONF_FILE_

#include <string>
using std::string;
class ConfFile {
    public:
	ConfFile(const char* filename, const char* mode);
	ConfFile();
	~ConfFile();

	void open(const char* filename, const char* mode);
	void close();

	bool read_int(string& name, int& val);
	bool read_string(string& name, string& val);
	bool read_bool(string& name,bool& val);

	bool write_int(const string& name, int val);
	bool write_string(const string& name, const string& val);
	bool write_bool(const string& name,bool val);	
	bool eof() { return feof(fp); }
    private:
	char* read_line_from_file(char* buf, int size, string& name);
    private:
	FILE* fp;
	static const int MAX_LEN;
};

#endif // _CONF_FILE_

