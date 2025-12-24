#ifndef PDF_UTILS_H
#define PDF_UTILS_H

#include <vector>
#include <string>
#include "../core/paper.h"

std::vector<std::string> list_pdfs(const std::string &folder_path);

bool is_added(const std::vector<paper>& papers,
              const std::string& filename,
              int &out_id);

void show_pdf_status(const std::vector<paper>& Paper,
                     const std::string &folder);

std::string set_pdf_path();

#endif // PDF_UTILS_H
