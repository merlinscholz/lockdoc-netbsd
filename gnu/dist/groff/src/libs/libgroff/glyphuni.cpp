/*	$NetBSD: glyphuni.cpp,v 1.1.1.1 2004/07/30 14:44:51 wiz Exp $	*/

// -*- C++ -*-
/* Copyright (C) 2002, 2003, 2004
   Free Software Foundation, Inc.
     Written by Werner Lemberg <wl@gnu.org>

This file is part of groff.

groff is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

groff is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with groff; see the file COPYING.  If not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#include "lib.h"
#include "stringclass.h"
#include "ptable.h"

#include "unicode.h"

struct glyph_to_unicode {
  char *value;
};

declare_ptable(glyph_to_unicode)
implement_ptable(glyph_to_unicode)

PTABLE(glyph_to_unicode) glyph_to_unicode_table;

struct S {
  const char *key;
  const char *value;
} glyph_to_unicode_list[] = {
  { "!", "0021" },
  { "\"", "0022" },
  { "dq", "0022" },
  { "#", "0023" },
  { "sh", "0023" },
  { "$", "0024" },
  { "Do", "0024" },
  { "%", "0025" },
  { "&", "0026" },
  { "aq", "0027" },
  { "(", "0028" },
  { ")", "0029" },
  { "*", "002A" },
  { "+", "002B" },
  { "pl", "002B" },
  { ",", "002C" },
  { ".", "002E" },
  { "/", "002F" },
  { "sl", "002F" },
  { "0", "0030" },
  { "1", "0031" },
  { "2", "0032" },
  { "3", "0033" },
  { "4", "0034" },
  { "5", "0035" },
  { "6", "0036" },
  { "7", "0037" },
  { "8", "0038" },
  { "9", "0039" },
  { ":", "003A" },
  { ";", "003B" },
  { "<", "003C" },
  { "=", "003D" },
  { "eq", "003D" },
  { ">", "003E" },
  { "?", "003F" },
  { "@", "0040" },
  { "at", "0040" },
  { "A", "0041" },
  { "B", "0042" },
  { "C", "0043" },
  { "D", "0044" },
  { "E", "0045" },
  { "F", "0046" },
  { "G", "0047" },
  { "H", "0048" },
  { "I", "0049" },
  { "J", "004A" },
  { "K", "004B" },
  { "L", "004C" },
  { "M", "004D" },
  { "N", "004E" },
  { "O", "004F" },
  { "P", "0050" },
  { "Q", "0051" },
  { "R", "0052" },
  { "S", "0053" },
  { "T", "0054" },
  { "U", "0055" },
  { "V", "0056" },
  { "W", "0057" },
  { "X", "0058" },
  { "Y", "0059" },
  { "Z", "005A" },
//{ "[", "005B" },
  { "lB", "005B" },
//{ "\\", "005C" },
  { "rs", "005C" },
//{ "]", "005D" },
  { "rB", "005D" },
  { "a^", "005E" },
  { "^", "005E" },
  { "ha", "005E" },
  { "_", "005F" },
  { "ru", "005F" },
  { "ul", "005F" },
//{ "\\`", "0060" },
  { "ga", "0060" },
  { "a", "0061" },
  { "b", "0062" },
  { "c", "0063" },
  { "d", "0064" },
  { "e", "0065" },
  { "f", "0066" },
  { "ff", "0066_0066" },
  { "Fi", "0066_0066_0069" },
  { "Fl", "0066_0066_006C" },
  { "fi", "0066_0069" },
  { "fl", "0066_006C" },
  { "g", "0067" },
  { "h", "0068" },
  { "i", "0069" },
  { "j", "006A" },
  { "k", "006B" },
  { "l", "006C" },
  { "m", "006D" },
  { "n", "006E" },
  { "o", "006F" },
  { "p", "0070" },
  { "q", "0071" },
  { "r", "0072" },
  { "s", "0073" },
  { "t", "0074" },
  { "u", "0075" },
  { "v", "0076" },
  { "w", "0077" },
  { "x", "0078" },
  { "y", "0079" },
  { "z", "007A" },
  { "lC", "007B" },
  { "{", "007B" },
  { "ba", "007C" },
  { "or", "007C" },
  { "|", "007C" },
  { "rC", "007D" },
  { "}", "007D" },
  { "a~", "007E" },
  { "~", "007E" },
  { "ti", "007E" },
  { "r!", "00A1" },
  { "ct", "00A2" },
  { "Po", "00A3" },
  { "Cs", "00A4" },
  { "Ye", "00A5" },
  { "bb", "00A6" },
  { "sc", "00A7" },
  { "ad", "00A8" },
  { "co", "00A9" },
  { "Of", "00AA" },
  { "Fo", "00AB" },
  { "no", "00AC" },
  { "tno", "00AC" },
  { "shc", "00AD" },
  { "rg", "00AE" },
  { "a-", "00AF" },
  { "de", "00B0" },
  { "+-", "00B1" },
  { "t+-", "00B1" },
  { "S2", "00B2" },
  { "S3", "00B3" },
  { "aa", "00B4" },
//{ "\\'", "00B4" },
  { "mc", "00B5" },
  { "ps", "00B6" },
  { "pc", "00B7" },
  { "ac", "00B8" },
  { "S1", "00B9" },
  { "Om", "00BA" },
  { "Fc", "00BB" },
  { "14", "00BC" },
  { "12", "00BD" },
  { "34", "00BE" },
  { "r?", "00BF" },
  { "`A", "00C0" },
  { "'A", "00C1" },
  { "^A", "00C2" },
  { "~A", "00C3" },
  { ":A", "00C4" },
  { "oA", "00C5" },
  { "AE", "00C6" },
  { ",C", "00C7" },
  { "`E", "00C8" },
  { "'E", "00C9" },
  { "^E", "00CA" },
  { ":E", "00CB" },
  { "`I", "00CC" },
  { "'I", "00CD" },
  { "^I", "00CE" },
  { ":I", "00CF" },
  { "-D", "00D0" },
  { "~N", "00D1" },
  { "`O", "00D2" },
  { "'O", "00D3" },
  { "^O", "00D4" },
  { "~O", "00D5" },
  { ":O", "00D6" },
  { "mu", "00D7" },
  { "tmu", "00D7" },
  { "/O", "00D8" },
  { "`U", "00D9" },
  { "'U", "00DA" },
  { "^U", "00DB" },
  { ":U", "00DC" },
  { "'Y", "00DD" },
  { "TP", "00DE" },
  { "ss", "00DF" },
  { "`a", "00E0" },
  { "'a", "00E1" },
  { "^a", "00E2" },
  { "~a", "00E3" },
  { ":a", "00E4" },
  { "oa", "00E5" },
  { "ae", "00E6" },
  { ",c", "00E7" },
  { "`e", "00E8" },
  { "'e", "00E9" },
  { "^e", "00EA" },
  { ":e", "00EB" },
  { "`i", "00EC" },
  { "'i", "00ED" },
  { "^i", "00EE" },
  { ":i", "00EF" },
  { "Sd", "00F0" },
  { "~n", "00F1" },
  { "`o", "00F2" },
  { "'o", "00F3" },
  { "^o", "00F4" },
  { "~o", "00F5" },
  { ":o", "00F6" },
  { "di", "00F7" },
  { "tdi", "00F7" },
  { "/o", "00F8" },
  { "`u", "00F9" },
  { "'u", "00FA" },
  { "^u", "00FB" },
  { ":u", "00FC" },
  { "'y", "00FD" },
  { "Tp", "00FE" },
  { ":y", "00FF" },
  { "'C", "0106" },
  { "'c", "0107" },
  { ".i", "0131" },
  { "IJ", "0132" },
  { "ij", "0133" },
  { "/L", "0141" },
  { "/l", "0142" },
  { "OE", "0152" },
  { "oe", "0153" },
  { "vS", "0160" },
  { "vs", "0161" },
  { ":Y", "0178" },
  { "vZ", "017D" },
  { "vz", "017E" },
  { "Fn", "0192" },
  { "ah", "02C7" },
  { "ab", "02D8" },
  { "a.", "02D9" },
  { "ao", "02DA" },
  { "ho", "02DB" },
  { "a\"", "02DD" },
  { "*A", "0391" },
  { "*B", "0392" },
  { "*G", "0393" },
  { "*D", "0394" },
  { "*E", "0395" },
  { "*Z", "0396" },
  { "*Y", "0397" },
  { "*H", "0398" },
  { "*I", "0399" },
  { "*K", "039A" },
  { "*L", "039B" },
  { "*M", "039C" },
  { "*N", "039D" },
  { "*C", "039E" },
  { "*O", "039F" },
  { "*P", "03A0" },
  { "*R", "03A1" },
  { "*S", "03A3" },
  { "*T", "03A4" },
  { "*U", "03A5" },
  { "*F", "03A6" },
  { "*X", "03A7" },
  { "*Q", "03A8" },
  { "*W", "03A9" },
  { "*a", "03B1" },
  { "*b", "03B2" },
  { "*g", "03B3" },
  { "*d", "03B4" },
  { "*e", "03B5" },
  { "*z", "03B6" },
  { "*y", "03B7" },
  { "*h", "03B8" },
  { "*i", "03B9" },
  { "*k", "03BA" },
  { "*l", "03BB" },
  { "*m", "03BC" },
  { "*n", "03BD" },
  { "*c", "03BE" },
  { "*o", "03BF" },
  { "*p", "03C0" },
  { "*r", "03C1" },
  { "ts", "03C2" },
  { "*s", "03C3" },
  { "*t", "03C4" },
  { "*u", "03C5" },
  { "*f", "03C6" },
  { "*x", "03C7" },
  { "*q", "03C8" },
  { "*w", "03C9" },
  { "+h", "03D1" },
  { "+f", "03D5" },
  { "+p", "03D6" },
  { "+e", "03F5" },
  { "-", "2010" },
  { "hy", "2010" },
  { "en", "2013" },
  { "em", "2014" },
  { "`", "2018" },
  { "oq", "2018" },
  { "'", "2019" },
  { "cq", "2019" },
  { "bq", "201A" },
  { "lq", "201C" },
  { "rq", "201D" },
  { "Bq", "201E" },
  { "dg", "2020" },
  { "dd", "2021" },
  { "bu", "2022" },
  { "%0", "2030" },
  { "fm", "2032" },
  { "sd", "2033" },
  { "fo", "2039" },
  { "fc", "203A" },
  { "rn", "203E" },
  { "f/", "2044" },
  { "eu", "20AC" },
  { "Eu", "20AC" },
  { "-h", "210F" },
  { "hbar", "210F" },
  { "Im", "2111" },
  { "wp", "2118" },
  { "Re", "211C" },
  { "tm", "2122" },
  { "Ah", "2135" },
  { "18", "215B" },
  { "38", "215C" },
  { "58", "215D" },
  { "78", "215E" },
  { "<-", "2190" },
  { "ua", "2191" },
  { "->", "2192" },
  { "da", "2193" },
  { "<>", "2194" },
  { "va", "2195" },
  { "CR", "21B5" },
  { "lA", "21D0" },
  { "uA", "21D1" },
  { "rA", "21D2" },
  { "dA", "21D3" },
  { "hA", "21D4" },
  { "vA", "21D5" },
  { "fa", "2200" },
  { "pd", "2202" },
  { "te", "2203" },
  { "es", "2205" },
  { "gr", "2207" },
  { "mo", "2208" },
  { "nm", "2209" },
  { "st", "220B" },
  { "product", "220F" },
  { "coproduct", "2210" },
  { "sum", "2211" },
//{ "\\-", "2212" },
  { "mi", "2212" },
  { "-+", "2213" },
  { "**", "2217" },
  { "sr", "221A" },
  { "pt", "221D" },
  { "if", "221E" },
  { "/_", "2220" },
  { "AN", "2227" },
  { "OR", "2228" },
  { "ca", "2229" },
  { "cu", "222A" },
  { "is", "222B" },
  { "integral", "222B" },
  { "tf", "2234" },
  { "3d", "2234" },
  { "ap", "223C" },
  { "|=", "2243" },
  { "=~", "2245" },
  { "~~", "2248" },
  { "~=", "2248" },
  { "!=", "2260" },
  { "==", "2261" },
  { "ne", "2262" },
  { "<=", "2264" },
  { ">=", "2265" },
  { ">>", "226A" },
  { "<<", "226B" },
  { "sb", "2282" },
  { "sp", "2283" },
  { "nb", "2284" },
  { "nc", "2285" },
  { "ib", "2286" },
  { "ip", "2287" },
  { "c+", "2295" },
  { "c*", "2297" },
  { "pp", "22A5" },
  { "md", "22C5" },
  { "lc", "2308" },
  { "rc", "2309" },
  { "lf", "230A" },
  { "rf", "230B" },
  { "parenlefttp", "239B" },
  { "parenleftex", "239C" },
  { "parenleftbt", "239D" },
  { "parenrighttp", "239E" },
  { "parenrightex", "239F" },
  { "parenrightbt", "23A0" },
  { "bracketlefttp", "23A1" },
  { "bracketleftex", "23A2" },
  { "bracketleftbt", "23A3" },
  { "bracketrighttp", "23A4" },
  { "bracketrightex", "23A5" },
  { "bracketrightbt", "23A6" },
  { "lt", "23A7" },
  { "bracelefttp", "23A7" },
  { "lk", "23A8" },
  { "braceleftmid", "23A8" },
  { "lb", "23A9" },
  { "braceleftbt", "23A9" },
  { "bv", "23AA" },
  { "braceex", "23AA" },
  { "braceleftex", "23AA" },
  { "bracerightex", "23AA" },
  { "rt", "23AB" },
  { "bracerighttp", "23AB" },
  { "rk", "23AC" },
  { "bracerightmid", "23AC" },
  { "rb", "23AD" },
  { "bracerightbt", "23AD" },
  { "an", "23AF" },
  { "br", "2502" },
  { "rk", "251D" },
  { "lk", "2525" },
  { "lt", "256D" },
  { "rt", "256E" },
  { "rb", "256F" },
  { "lb", "2570" },
  { "sq", "25A1" },
  { "lz", "25CA" },
  { "ci", "25CB" },
  { "lh", "261C" },
  { "rh", "261E" },
  { "SP", "2660" },
  { "CL", "2663" },
  { "HE", "2665" },
  { "DI", "2666" },
  { "OK", "2713" },
  { "la", "27E8" },
  { "ra", "27E9" },
};

// global constructor
static struct glyph_to_unicode_init {
  glyph_to_unicode_init();
} _glyph_to_unicode_init;

glyph_to_unicode_init::glyph_to_unicode_init() {
  for (unsigned int i = 0;
       i < sizeof(glyph_to_unicode_list)/sizeof(glyph_to_unicode_list[0]);
       i++) {
    glyph_to_unicode *gtu = new glyph_to_unicode[1];
    gtu->value = (char *)glyph_to_unicode_list[i].value;
    glyph_to_unicode_table.define(glyph_to_unicode_list[i].key, gtu);
  }
}

const char *glyph_name_to_unicode(const char *s)
{
  glyph_to_unicode *result = glyph_to_unicode_table.lookup(s);
  return result ? result->value : 0;
}
