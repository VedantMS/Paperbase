#ifndef PAPER_H
#define PAPER_H

#include <string>
#include <vector>

struct Author {
    std::string first;
    std::string middle;
    std::string last;
};

class paper {

    public:
        int id;
        std::string title;
        std::vector<Author> authors;
        std::string keywords;
        std::string date;
        std::string url;
        std::string doi;
        std::string journal_name;
        std::string pdf_path;
        std::string tags;
};

#endif // PAPER_H
