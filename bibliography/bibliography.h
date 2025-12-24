#ifndef BIBLIOGRAPHY_H
#define BIBLIOGRAPHY_H

#include <string>
#include <vector>
#include "../core/paper.h"

enum class BibFormat {
    PLAIN,
    SPRINGER,
    IEEE,
    APA,
    CHICAGO,
    MLA,
    BIBTEX
};

std::string generateBibliography(
    const std::vector<paper> &papers,
    BibFormat format,
    bool latexMode
    );

#endif // BIBLIOGRAPHY_H
