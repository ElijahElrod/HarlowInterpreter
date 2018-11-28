#ifndef __COMMANDFUNCTIONS_H
#define __COMMANDFUNCTIONS_H

#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "storytokenizer.hpp"

bool hasLinks(vector<string> name);
void callBlock(SectionToken stok, bool flag1, bool& flag2);
void callGoTo(SectionToken stok, string& redirect, bool& flag);
void callLink(SectionToken stok, vector<string>& links, vector<string>& redirectLinks);
void callIf(StoryGuide narrator, SectionToken stok, bool& flag1);
void callElseIf(StoryGuide narrator, SectionToken stok, bool& flag1);
void callElse(bool& flag1);
void continueStory(vector<string>& links, vector<string> redirectLinks, string& passToGo, string passageRedirect, bool flag3);

#endif