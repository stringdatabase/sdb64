* SDPYFUNC.H
* SD Python Object Functions  
* Copyright (c)2025 The SD Developers, All Rights Reserved
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software Foundation,
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*
* START-HISTORY:
* rev 0.9-2 Mar 25 mab 
* END-HISTORY
*
* START-DESCRIPTION:
*
* END-DESCRIPTION
*
* START-CODE

deffun PY_CREATEDICT(dictname) calling '!PY_CREATEDICT'
deffun PY_CLEARDICT(dictname) calling '!PY_CLEARDICT'
deffun PY_DICTVALGETS(dictname,key) calling '!PY_DICTVALGETS'
deffun PY_DICTVALSETS(dictname,key,value) calling '!PY_DICTVALSETS'
