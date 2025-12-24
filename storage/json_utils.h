#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "../core/paper.h"

using json = nlohmann::json;

json paper_to_json(const paper &p);

paper json_to_paper(const json &p);

void save_json(const std::vector<paper> &Paper,
               const std::string &filename,
               const std::string &pdf_folder);

std::vector<paper> load_json(const std::string &filename,
                             std::string &pdf_folder);

#endif // JSON_UTILS_H
