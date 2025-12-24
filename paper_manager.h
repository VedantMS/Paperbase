#ifndef PAPER_MANAGER_H
#define PAPER_MANAGER_H

#include <vector>
#include <string>
#include "paper.h"

void add_paper(std::vector<paper>& Paper,
               const std::string &folder_path,
               int &next_id);

void display(std::vector<paper> &Paper);

#endif
