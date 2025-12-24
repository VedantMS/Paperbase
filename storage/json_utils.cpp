#include<fstream>
#include "json_utils.h"

json paper_to_json(const paper &p) {

    json authorsJson = json::array();

    for (const auto& a : p.authors) {
        authorsJson.push_back({
            {"first", a.first},
            {"middle", a.middle},
            {"last", a.last}
        });
    }
    return json{
        {"id", p.id},
        {"title", p.title},
        {"authors", authorsJson},
        {"keywords", p.keywords},
        {"date", p.date},
        {"url", p.url},
        {"doi", p.doi},
        {"journal_name", p.journal_name},
        {"tags", p.tags},
        {"pdf_path", p.pdf_path}
    };
}

paper json_to_paper(const json &j) {
    paper p;

    p.id			= j.value("id", 0);
    p.title			= j.value("title", "");
    p.authors.clear();
    if (j.contains("authors") && j["authors"].is_array()) {
        for (const auto& a : j["authors"]) {
            Author author;
            author.first  = a.value("first", "");
            author.middle = a.value("middle", "");
            author.last   = a.value("last", "");
            p.authors.push_back(author);
        }
    }

    p.keywords		= j.value("keywords", "");
    p.date			= j.value("date", "");
    p.url			= j.value("url", "");
    p.doi			= j.value("doi", "");
    p.journal_name	= j.value("journal_name", "");
    p.tags			= j.value("tags", "");
    p.pdf_path		= j.value("pdf_path", "");

    return p;
}

void save_json(const std::vector<paper> &Paper, const std::string &filename, const std::string &pdf_folder) {

    json j;
    j["pdf_folder"] = pdf_folder;

    j["papers"] = json::array();

    for (const auto &p : Paper) {
        j["papers"].push_back(paper_to_json(p));
    }

    std::ofstream file(filename);
    file << j.dump(4);
    file.close();
}

std::vector<paper> load_json(const std::string &filename, std::string &pdf_folder) {

    std::ifstream file(filename);
    json j;
    file >> j;

    pdf_folder = j.value("pdf_folder", "");

    std::vector<paper> papers;
    for (auto &item : j["papers"])
        papers.push_back(json_to_paper(item));

    return papers;
}
