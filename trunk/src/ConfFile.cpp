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

#include "ConfFile.h"

#include <stdexcept>
#include <errno.h>
using std::runtime_error;

const int ConfFile::MAX_LEN = 512;

ConfFile::ConfFile(const char* filename, const char* mode) :
    fp(NULL)
{
    open(filename, mode);
}

ConfFile::ConfFile() :
    fp(NULL)
{}

ConfFile::~ConfFile()
{
    try {
	close();
    }
    catch (...){}
}

void ConfFile::open(const char* filename, const char* mode)
{
    close();
    if (NULL == (fp = fopen(filename, mode)))
	throw runtime_error(strerror(errno));
}

void ConfFile::close()
{
    if (NULL != fp) {
	if (0 != fclose(fp))
	    throw runtime_error(strerror(errno));
    }
    fp = NULL;
}

bool ConfFile::write_int(const string& name, int val)
{
    if (NULL != fp)
	return 0 != fprintf(fp, "%s = %d\n", name.c_str(), val);
    return false;
}

bool ConfFile::write_string(const string& name, const string& val)
{
    if (NULL != fp)
	return 0 != fprintf(fp, "%s = %s\n", name.c_str(), val.c_str());
    return false;
}
bool ConfFile::write_bool(const string& name,bool val)
{
	string tmp="FALSE";
	if(val==true)
		tmp="TRUE";
	return write_string(name,tmp);
}

bool ConfFile::read_int(string& name, int& val)
{
    char buf[MAX_LEN];
    char* cur;
    if (NULL != (cur = read_line_from_file(buf, MAX_LEN, name))) {
	if (*cur) {
	    val = atoi(cur);
	    return true;
	}
    }
    return false;
}

bool ConfFile::read_string(string& name, string& var)
{
    char buf[MAX_LEN];
    char* cur;
    if (NULL != (cur = read_line_from_file(buf, MAX_LEN, name))) {
	if (*cur) {
	    char* max = cur;
	    while (*max && (*max != '\n')) // 去掉结未的换行
		max++;
	    *max = '\0';
	    var.assign(cur, ++max);
	} else
	    var.erase();
	return true;
    }
    return false;
}
bool ConfFile::read_bool(string& name,bool & var)
{
	string tmp;
	read_string(name,tmp);
	if(tmp=="TRUE")
		var=true;
	else
		if(tmp=="FALSE")
			var=false;
		else
			return true;
	return false;			
}
char* ConfFile::read_line_from_file(char* buf, int size, string& name)
{
    if (size < 2 || eof()) {
	name.erase();
	return NULL;
    }
    
    buf [size - 2] = '\0';
    if (fgets(buf, size, fp) && ('\0' == buf [size - 2])) {
	char* cur = buf;
	while (*cur && !isspace(*cur))
	    cur++; 
	if (*cur) {
	    name.assign(buf, cur);
	} else {
	    name.erase();
	    return NULL;
	}
	while (*cur && (('=' == *cur) || isspace(*cur)))
	    cur++;
	return cur;
    }
    // 如果最后第2位改变了，说明没有读完一行
    return NULL;
}


