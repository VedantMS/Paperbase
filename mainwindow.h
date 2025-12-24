#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qlistwidget.h>
//#include <QListWidgetItem>
#include<vector>
#include<string>
#include "core/paper.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private slots:
        void on_setPdfFolderButton_clicked();
        void on_refreshPdfListButton_clicked();
        void on_addPaperButton_clicked();
        void on_showPapersButton_clicked();
        void on_saveButton_clicked();
        void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
        void on_generateBibliographyButton_clicked();
        void on_backButton_clicked();
        void on_copyButton_clicked();
        void on_exportButton_clicked();
        void on_listWidget_itemSelectionChanged();
        void on_outputModeChanged();
        void on_regenerateButton_clicked();
        void on_comboBox_currentTextChanged(const QString &);

    private:
        Ui::MainWindow *ui;
        std::vector<paper> Paper;
        std::vector<paper> getSelectedPapers() const;
        std::string pdf_folder;
        int next_id = 1;

        void regenerateBibliography();
        void updateCurrentPaperFromUI();

};

#endif // MAINWINDOW_H
