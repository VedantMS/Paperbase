#include "../bibliography/bibliography.h"
#include <sstream>

static std::string surname_with_initials(const Author& a)
{
    std::ostringstream out;

    if (!a.last.empty())
        out << a.last;

    if (!a.first.empty())
        out << ", " << a.first[0] << ".";

    if (!a.middle.empty())
        out << " " << a.middle[0] << ".";

    return out.str();
}

static std::string full_author_name(const Author& a)
{
    std::ostringstream out;
    out << a.first << " ";
    if (!a.middle.empty())
        out << a.middle << " ";
    out << a.last;
    return out.str();
}

static std::string apa_author(const Author& a)
{
    std::ostringstream out;
    out << a.last << ", ";

    if (!a.first.empty())
        out << a.first[0] << ". ";

    if (!a.middle.empty())
        out << a.middle[0] << ". ";

    return out.str();
}

static std::string format_plain(const paper& p, int index)
{
    std::ostringstream out;
    out << "[" << index << "] ";

    for (size_t i = 0; i < p.authors.size(); ++i) {
        out << full_author_name(p.authors[i]);
        if (i + 1 < p.authors.size())
            out << "; ";
    }

    out << ". " << p.title << ". ";

    if (!p.journal_name.empty())
        out << p.journal_name << ", ";

    out << p.date << ".";

    return out.str();
}

static std::string format_ieee(const paper& p, int index)
{
    std::ostringstream out;
    out << "[" << index << "] ";

    for (size_t i = 0; i < p.authors.size(); ++i) {
        out << surname_with_initials(p.authors[i]);
        if (i + 1 < p.authors.size())
            out << ", ";
    }

    out << ", \"" << p.title << "\", ";

    if (!p.journal_name.empty())
        out << p.journal_name << ", ";

    out << p.date;

    if (!p.doi.empty())
        out << ", DOI: " << p.doi;

    if (!p.url.empty())
        out << ", URL: " << p.url;

    out << ".";

    return out.str();
}

static std::string format_apa(const paper& p)
{
    std::ostringstream out;

    for (size_t i = 0; i < p.authors.size(); ++i) {
        out << p.authors[i].last << ", "
            << p.authors[i].first[0] << ".";

        if (!p.authors[i].middle.empty())
            out << " " << p.authors[i].middle[0] << ".";

        if (i + 2 < p.authors.size())
            out << ", ";
        else if (i + 1 < p.authors.size())
            out << ", & ";
    }

    out << " (" << p.date << "). "
        << p.title << ". ";

    if (!p.journal_name.empty())
        out << p.journal_name << ". ";

    if (!p.doi.empty())
        out << p.doi;

    if (!p.url.empty())
        out << p.url;

    return out.str();
}

static std::string format_springer(const paper &p, int index)
{
    std::ostringstream out;

    out << "[" << index << "] ";

    for (size_t i = 0; i < p.authors.size(); ++i) {

        out << surname_with_initials(p.authors[i]);
        if (i + 1 < p.authors.size())
            out << ", ";
    }

    out << " (" << p.date << ") "
        << p.title << ". ";

    if (!p.journal_name.empty())
        out << p.journal_name << ".";

    return out.str();
}

static std::string format_chicago(const paper& p)
{
    std::ostringstream out;

    out << full_author_name(p.authors.front()) << ". "
        << p.title << ". ";

    if (!p.journal_name.empty())
        out << p.journal_name << ", ";

    out << p.date << ".";

    return out.str();
}

static std::string format_mla(const paper& p)
{
    if (p.authors.empty())
        return "[Invalid MLA entry: no authors]";

    const Author& a = p.authors.front();

    std::ostringstream out;

    if (!a.last.empty()) {
        out << a.last;
        if (!a.first.empty())
            out << ", " << a.first;
        out << ". ";
    }

    out << p.title << ". ";

    if (!p.journal_name.empty())
        out << p.journal_name << ", ";

    out << p.date << ".";

    return out.str();
}

static std::string make_bibtex_key(const paper& p)
{
    std::string key = "paper";

    if (!p.authors.empty() && !p.authors.front().last.empty())
        key = p.authors.front().last;

    if (!p.date.empty())
        key += "_" + p.date;

    return key;
}

static std::string format_bibtex(const paper& p)
{
    std::ostringstream out;
    std::string key = make_bibtex_key(p);

    out << "@article{" << key << ",\n"
        << "  title = {" << p.title << "},\n"
        << "  author = {";

    for (size_t i = 0; i < p.authors.size(); ++i) {
        out << p.authors[i].last << ", " << p.authors[i].first;
        if (i + 1 < p.authors.size())
            out << " and ";
    }

    out << "},\n";

    if (!p.journal_name.empty())
        out << "  journal = {" << p.journal_name << "},\n";

    if (!p.date.empty())
        out << "  year = {" << p.date << "},\n";

    if (!p.doi.empty())
        out << "  doi = {" << p.doi << "},\n";

    if (!p.url.empty())
        out << "  url = {" << p.url << "},\n";

    out << "}\n";
    return out.str();
}

static std::string wrap_latex_bibitem(
    const paper& p,
    const std::string& entry)
{
    std::ostringstream out;

    std::string key = "paper";

    if (!p.authors.empty() && !p.authors.front().last.empty())
        key = p.authors.front().last;

    if (!p.date.empty())
        key += "_" + p.date;

    out << "\\bibitem{" << key << "} " << entry;
    return out.str();
}


std::string generateBibliography(
    const std::vector<paper>& papers,
    BibFormat format,
    bool latexMode)
{
    std::ostringstream out;

    for (size_t i = 0; i < papers.size(); ++i) {
        const paper& p = papers[i];
        std::string entry;

        switch (format) {
        case BibFormat::PLAIN:
            entry = format_plain(p, i + 1);
            break;
        case BibFormat::IEEE:
            entry = format_ieee(p, i + 1);
            break;
        case BibFormat::APA:
            entry = format_apa(p);
            break;
        case BibFormat::SPRINGER:
            entry = format_springer(p, i + 1);
            break;
        case BibFormat::CHICAGO:
            entry = format_chicago(p);
            break;
        case BibFormat::MLA:
            entry = format_mla(p);
            break;
        case BibFormat::BIBTEX:
            entry = format_bibtex(p);
            break;
        }

        if (latexMode && format != BibFormat::BIBTEX)
            entry = wrap_latex_bibitem(p, entry);

        out << entry << "\n\n";
    }

    return out.str();
}
