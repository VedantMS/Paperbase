#include "mainwindow.h"
#include "bibliography/bibliography.h"
#include "ui_mainwindow.h"

#include <QClipboard>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>


#include "core/paper.h"

std::vector<std::string> list_pdfs(const std::string &folder_path);
bool is_added(const std::vector<paper>& papers, const std::string& filename, int &out_id);
void save_json(const std::vector<paper>& Paper, const std::string &filename, const std::string &pdf_folder);
std::vector<paper> load_json(const std::string &filename, std::string &pdf_folder);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    ui->showPapersButton->setEnabled(false);
    ui->generateBibliographyButton->setEnabled(false);
    ui->saveButton->setEnabled(false);
    ui->addPaperButton->setEnabled(false);

    ui->groupBox->setVisible(false);

    this->setWindowIcon(QIcon(":/icons/app.ico"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_setPdfFolderButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        "Select PDF Root Folder"
        );

    if (dir.isEmpty())
        return;

    pdf_folder = dir.toStdString();

    Paper.clear();
    next_id = 1;

    ui->showPapersButton->setEnabled(false);
    ui->generateBibliographyButton->setEnabled(false);

    QString jsonPath = dir + "/papers.json";
    QFile f(jsonPath);
    if (f.exists()) {
        Paper = load_json(jsonPath.toStdString(), pdf_folder);
        if (!Paper.empty())
            next_id = Paper.back().id + 1;
    }

    if (!Paper.empty())
        ui->showPapersButton->setEnabled(true);
    else
        ui->showPapersButton->setEnabled(false);

    on_refreshPdfListButton_clicked();
}

void MainWindow::on_refreshPdfListButton_clicked()
{
    ui->listWidget->clear();

    if (pdf_folder.empty())
        return;

    std::vector<std::string> pdfs = list_pdfs(pdf_folder);

    for (const auto &pdf : pdfs) {
        int id = -1;
        QString label;

        if (is_added(Paper, pdf, id))
            label = QString::fromStdString(pdf) + "  [ID: " + QString::number(id) + "]";
        else
            label = QString::fromStdString(pdf) + "  [NOT ADDED]";

        ui->listWidget->addItem(label);
    }
}

void MainWindow::on_addPaperButton_clicked()
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Error", "Select a PDF first");
        return;
    }

    QString text = item->text();
    if (text.contains("ID:")) {
        QMessageBox::information(this, "Info", "PDF already added");
        return;
    }

    paper p;
    p.id = next_id++;

    p.title = ui->lineEdit->text().toStdString();
    QString authorsText = ui->lineEdit_2->text();
    QStringList list = authorsText.split(";", Qt::SkipEmptyParts);

    for (const QString& a : list) {
        QStringList parts = a.trimmed().split(" ", Qt::SkipEmptyParts);

        Author author;
        if (parts.size() >= 1) author.first = parts[0].toStdString();
        if (parts.size() >= 2) author.last  = parts.back().toStdString();
        if (parts.size() > 2) {
            author.middle.clear();
            for (int i = 1; i < parts.size() - 1; ++i)
                author.middle += parts[i].toStdString() + " ";
        }

        p.authors.push_back(author);
    }

    p.keywords = ui->lineEdit_3->text().toStdString();
    p.date = ui->lineEdit_4->text().toStdString();
    p.journal_name = ui->lineEdit_5->text().toStdString();
    p.url = ui->lineEdit_6->text().toStdString();
    p.doi = ui->lineEdit_7->text().toStdString();
    p.tags = ui->lineEdit_8->text().toStdString();

    QString filename = text.split("  ").first();
    p.pdf_path = pdf_folder + "/" + filename.toStdString();

    Paper.push_back(p);

    on_refreshPdfListButton_clicked();
    ui->addPaperButton->setEnabled(false);
    ui->saveButton->setEnabled(true);
}

void MainWindow::on_showPapersButton_clicked()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(6);

    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setHorizontalHeaderLabels(
        {" ", "ID", "Title", "Authors", "Date", "Journal Name"}
        );

    for (size_t i = 0; i < Paper.size(); i++) {
        ui->tableWidget->insertRow(i);

        QTableWidgetItem *checkItem = new QTableWidgetItem();
        checkItem->setCheckState(Qt::Unchecked);
        checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

        ui->tableWidget->setItem(i, 0, checkItem);
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(Paper[i].id)));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(Paper[i].title)));
        QString authors;

        for (const auto& a : Paper[i].authors) {
            QString name;

            if (!a.first.empty())
                name += QString::fromStdString(a.first) + " ";

            if (!a.middle.empty())
                name += QString::fromStdString(a.middle) + " ";

            if (!a.last.empty())
                name += QString::fromStdString(a.last);

            authors += name.trimmed() + "; ";
        }

        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(authors));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(Paper[i].date)));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(Paper[i].journal_name)));
    }

    ui->generateBibliographyButton->setEnabled(true);
}

void MainWindow::updateCurrentPaperFromUI()
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (!item)
        return;

    QString text = item->text();
    QString filename = text.split("  ").first();

    int id = -1;
    if (!is_added(Paper, filename.toStdString(), id))
        return;

    for (auto &p : Paper) {
        if (p.id == id) {

            p.authors.clear();
            QString authorsText = ui->lineEdit_2->text();
            QStringList list = authorsText.split(";", Qt::SkipEmptyParts);

            for (const QString& a : list) {
                QStringList parts = a.trimmed().split(" ", Qt::SkipEmptyParts);
                if (parts.isEmpty())
                    continue;

                Author author;
                author.first = parts[0].toStdString();
                if (parts.size() >= 2)
                    author.last = parts.last().toStdString();

                if (parts.size() > 2) {
                    for (int i = 1; i < parts.size() - 1; ++i)
                        author.middle += parts[i].toStdString() + " ";
                }

                p.authors.push_back(author);
            }

            p.title        = ui->lineEdit->text().toStdString();
            p.keywords     = ui->lineEdit_3->text().toStdString();
            p.date         = ui->lineEdit_4->text().toStdString();
            p.journal_name = ui->lineEdit_5->text().toStdString();
            p.url          = ui->lineEdit_6->text().toStdString();
            p.doi          = ui->lineEdit_7->text().toStdString();
            p.tags         = ui->lineEdit_8->text().toStdString();

            break;
        }
    }
}

void MainWindow::on_saveButton_clicked()
{
    if (pdf_folder.empty()) {
        QMessageBox::warning(this, "Error", "No PDF folder set");
        return;
    }

    updateCurrentPaperFromUI();

    save_json(Paper, pdf_folder + "/papers.json", pdf_folder);

    QMessageBox::information(this, "Saved",
                             "Changes saved successfully");
}

static QString authorsToQString(const std::vector<Author>& authors)
{
    QStringList list;

    for (const auto& a : authors) {
        QString name = QString::fromStdString(a.first);

        if (!a.middle.empty())
            name += " " + QString::fromStdString(a.middle);

        name += " " + QString::fromStdString(a.last);
        list << name;
    }

    return list.join("; ");
}

void MainWindow::on_listWidget_itemSelectionChanged()
{    
    ui->groupBox->setVisible(true);
    ui->groupBox->setEnabled(true);
    ui->addPaperButton->setEnabled(false);
    ui->saveButton->setEnabled(false);

    QListWidgetItem *item = ui->listWidget->currentItem();
    if (!item)
        return;

    QString text = item->text();
    QString filename = text.split("  ").first();

    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
    ui->lineEdit_5->clear();
    ui->lineEdit_6->clear();
    ui->lineEdit_7->clear();
    ui->lineEdit_8->clear();

    int id = -1;

    if (is_added(Paper, filename.toStdString(), id)) {
        ui->saveButton->setEnabled(true);
        for (auto &p : Paper) {
            if (p.id == id) {
                ui->lineEdit->setText(QString::fromStdString(p.title));
                ui->lineEdit_2->setText(authorsToQString(p.authors));
                ui->lineEdit_3->setText(QString::fromStdString(p.keywords));
                ui->lineEdit_4->setText(QString::fromStdString(p.date));
                ui->lineEdit_5->setText(QString::fromStdString(p.journal_name));
                ui->lineEdit_6->setText(QString::fromStdString(p.url));
                ui->lineEdit_7->setText(QString::fromStdString(p.doi));
                ui->lineEdit_8->setText(QString::fromStdString(p.tags));
                break;
            }
        }
    }

   else {
        ui->addPaperButton->setEnabled(true);
        ui->saveButton->setEnabled(false);
    }
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if (!item || pdf_folder.empty())
        return;

    QString filename = item->text().split("  ").first();
    QString fullPath = QString::fromStdString(pdf_folder) + "/" + filename;

    QDesktopServices::openUrl(QUrl::fromLocalFile(fullPath));
}

void MainWindow::regenerateBibliography()
{
    auto selected = getSelectedPapers();
    if (selected.empty())
        return;

    BibFormat format = BibFormat::PLAIN;
    QString choice = ui->comboBox->currentText();

    if (choice == "IEEE")
        format = BibFormat::IEEE;
    else if (choice == "APA")
        format = BibFormat::APA;
    else if (choice == "Springer")
        format = BibFormat::SPRINGER;
    else if (choice == "MLA")
        format = BibFormat::MLA;
    else if (choice == "BibTeX")
        format = BibFormat::BIBTEX;

    bool latexMode = ui->latexButton->isChecked();

    std::string result =
        generateBibliography(selected, format, latexMode);

    ui->bibliographyTextEdit->setPlainText(
        QString::fromStdString(result)
        );
}

void MainWindow::on_generateBibliographyButton_clicked()
{
    auto selected = getSelectedPapers();

    if (selected.empty()) {
        QMessageBox::warning(this, "No selection",
                             "Please select at least one paper.");
        return;
    }

    ui->stackedWidget->setCurrentIndex(1);

    regenerateBibliography();
}

void MainWindow::on_comboBox_currentTextChanged(const QString &)
{
    if (ui->stackedWidget->currentIndex() != 1)
        return;
}

void MainWindow::on_regenerateButton_clicked()
{
    if (ui->stackedWidget->currentIndex() != 1)
        return;

    regenerateBibliography();
}

void MainWindow::on_backButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);

}

void MainWindow::on_copyButton_clicked()
{
    QString text;
    text = ui->bibliographyTextEdit->toPlainText();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);

    QMessageBox::information(this, "Copied",
                             "Bibliography copied to clipboard.");
}

void MainWindow::on_exportButton_clicked()
{
    QString text = ui->bibliographyTextEdit->toPlainText();

    QString folder = QFileDialog::getExistingDirectory(
        this,
        "Select Export Folder"
        );

    if (folder.isEmpty())
        return;

    QString filePath = folder + "/bibliography.txt";

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error",
                             "Could not write bibliography file.");
        return;
    }

    QTextStream out(&file);
    out << text;
    file.close();

    QMessageBox::information(
        this,
        "Exported",
        "Bibliography exported to:\n" + filePath
        );
}

void MainWindow::on_outputModeChanged()
{
    if (ui->stackedWidget->currentIndex() != 1)
        return;

    auto selected = getSelectedPapers();
    if (selected.empty())
        return;

    BibFormat format = BibFormat::PLAIN;
    QString choice = ui->comboBox->currentText();

    if (choice == "IEEE") format = BibFormat::IEEE;
    else if (choice == "APA") format = BibFormat::APA;
    else if (choice == "Springer") format = BibFormat::SPRINGER;
    else if (choice == "MLA") format = BibFormat::MLA;
    else if (choice == "BibTeX") format = BibFormat::BIBTEX;

    bool latexMode = ui->latexButton->isChecked();

    ui->bibliographyTextEdit->setPlainText(
        QString::fromStdString(
            generateBibliography(selected, format, latexMode)
            )
        );
}

std::vector<paper> MainWindow::getSelectedPapers() const
{
    std::vector<paper> selected;

    for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
        QTableWidgetItem *item = ui->tableWidget->item(row, 0);
        if (item && item->checkState() == Qt::Checked) {
            selected.push_back(Paper[row]);
        }
    }
    return selected;
}



