// montablewidget.cpp

#include "montabwidget.h"
#include "project.h"

MonTabWidget::MonTabWidget(Project *project, QWidget *parent) : QTabWidget(parent) {
    //
    this->project = project;
    populate();
    installEventFilter(this);
}

bool MonTabWidget::eventFilter(QObject *object, QEvent *event) {
    //
    // press right mouse button
    if (event->type() == QEvent::MouseButtonPress
     && static_cast<QMouseEvent *>(event)->button() == Qt::RightButton) {
        //
        QPoint eventPos = static_cast<QMouseEvent *>(event)->pos();
        int tabIndex = tabBar()->tabAt(eventPos);
        if (tabIndex > -1) {
            askActivateTab(tabIndex, eventPos);
        }
        //
    } else {
        //
    }
}

void MonTabWidget::populate() {
    //QPushButton *newTabButton = new QPushButton("Configure JSON...");
    //QObject::connect(newTabButton, &QPushButton::clicked, [=](){
    //    // TODO
    //    qDebug() << "configure json pressed";
    //});
    //tabWidget->setCornerWidget(newTabButton);

    // delete everything in the tab widget here? no

    Fields fields = project->wildMonFields;
    activeTabs = QVector<bool>(fields.size(), false);

    for (QPair<QString, QVector<int>> field : fields) {
        QTableWidget *table = new QTableWidget;
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setFocusPolicy(Qt::NoFocus);
        table->setSelectionMode(QAbstractItemView::NoSelection);
        table->clearFocus();
        addTab(table, field.first);
    }
}

void MonTabWidget::askActivateTab(int tabIndex, QPoint menuPos) {
    if (activeTabs[tabIndex]) return;

    QMenu contextMenu(this);

    QString tabText = tabBar()->tabText(tabIndex);
    QAction actionActivateTab(QString("Add %1 data for this map...").arg(tabText), this);
    connect(&actionActivateTab, &QAction::triggered, [=](){
        //
        //qDebug() << "activate tab" << tabIndex;
        clearTableAt(tabIndex);
        populateTab(tabIndex, getDefaultMonInfo(project->wildMonFields.at(tabIndex)), tabText);
        setCurrentIndex(tabIndex);
    });//SIGNAL(triggered()), this, SLOT(removeDataPoint()));
    contextMenu.addAction(&actionActivateTab);

    contextMenu.exec(mapToGlobal(menuPos));
}

void MonTabWidget::clearTableAt(int tabIndex) {
    QTableWidget *table = tableAt(tabIndex);
    if (table) {
        table->clear();
        table->horizontalHeader()->hide();
    }
}

void MonTabWidget::populateTab(int tabIndex, WildMonInfo monInfo, QString fieldName) {
    //
    int i = 1;

    //ui->stackedWidget_WildMons->insertWidget(0, speciesTable);
    //return;

    QTableWidget *speciesTable = tableAt(tabIndex);

    speciesTable->setRowCount(monInfo.wildPokemon.size());
    speciesTable->setColumnCount(6);// TODO: stretch last column?

    QStringList landMonTableHeaders;
    landMonTableHeaders << "Index" << "Species" << "Min Level" << "Max Level" << "Index Percentage" << "Encounter Rate";
    speciesTable->setHorizontalHeaderLabels(landMonTableHeaders);
    speciesTable->horizontalHeader()->show();
    speciesTable->verticalHeader()->hide();
    speciesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    speciesTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    speciesTable->setShowGrid(false);

    // set encounter rate slider
    // don't forget to add number label next to it
    QFrame *encounterFrame = new QFrame;
    QHBoxLayout *encounterLayout = new QHBoxLayout;

    QSlider *encounterRate = new QSlider(Qt::Horizontal);
    encounterRate->setMinimum(1);
    encounterRate->setMaximum(100);

    QLabel *encounterLabel = new QLabel;
    connect(encounterRate, &QSlider::valueChanged, [=](int value){
        encounterLabel->setText(QString::number(value));
    });
    encounterRate->setValue(monInfo.encounterRate);

    encounterLayout->addWidget(encounterLabel);
    encounterLayout->addWidget(encounterRate);

    encounterFrame->setLayout(encounterLayout);

    speciesTable->setCellWidget(0, 5, encounterFrame);

    for (WildPokemon mon : monInfo.wildPokemon) {
        createSpeciesTableRow(speciesTable, mon, i, fieldName);
        i++;
    }
    this->setTabActive(tabIndex, true);
}

// TODO: just move these funcs to editor.cpp
void MonTabWidget::createSpeciesTableRow(QTableWidget *table, WildPokemon mon, int index, QString fieldName) {
    //
    QPixmap monIcon = QPixmap(project->speciesToIconPath.value(mon.species)).copy(0, 0, 32, 32);

    QLabel *monNum = new QLabel(QString("%1.").arg(QString::number(index)));

    QLabel *monLabel = new QLabel();
    monLabel->setPixmap(monIcon);

    QComboBox *monSelector = new QComboBox;
    monSelector->setMinimumContentsLength(20);
    monSelector->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
    monSelector->addItems(project->speciesToIconPath.keys());
    monSelector->setCurrentText(mon.species);
    monSelector->setEditable(true);

    QObject::connect(monSelector, &QComboBox::currentTextChanged, [=](QString newSpecies){
        QPixmap monIcon = QPixmap(project->speciesToIconPath.value(newSpecies)).copy(0, 0, 32, 32);
        monLabel->setPixmap(monIcon);
    });

    QSpinBox *minLevel = new QSpinBox;
    QSpinBox *maxLevel = new QSpinBox;
    minLevel->setMinimum(1);
    minLevel->setMaximum(100);
    maxLevel->setMinimum(1);
    maxLevel->setMaximum(100);
    minLevel->setValue(mon.minLevel);
    maxLevel->setValue(mon.maxLevel);

    // connect level spinboxes so max is never less than min
    connect(minLevel, QOverload<int>::of(&QSpinBox::valueChanged), [maxLevel](int min){
        maxLevel->setMinimum(min);
    });

    int fieldIndex = 0;
    for (auto field : project->wildMonFields) {
        if (field.first == fieldName) break;
        fieldIndex++;
    }
    QLabel *percentLabel = new QLabel(QString("%1%").arg(
        QString::number(project->wildMonFields[fieldIndex].second[index - 1]
    )));

    QFrame *speciesSelector = new QFrame;
    QHBoxLayout *speciesSelectorLayout = new QHBoxLayout;
    speciesSelectorLayout->addWidget(monLabel);
    speciesSelectorLayout->addWidget(monSelector);
    speciesSelector->setLayout(speciesSelectorLayout);

    // prevent the spinboxes from being stupidly tall
    QFrame *minLevelFrame = new QFrame;
    QVBoxLayout *minLevelSpinboxLayout = new QVBoxLayout;
    minLevelSpinboxLayout->addWidget(minLevel);
    minLevelFrame->setLayout(minLevelSpinboxLayout);
    QFrame *maxLevelFrame = new QFrame;
    QVBoxLayout *maxLevelSpinboxLayout = new QVBoxLayout;
    maxLevelSpinboxLayout->addWidget(maxLevel);
    maxLevelFrame->setLayout(maxLevelSpinboxLayout);

    // add widgets to the table
    table->setCellWidget(index - 1, 0, monNum);
    table->setCellWidget(index - 1, 1, speciesSelector);
    table->setCellWidget(index - 1, 2, minLevelFrame);
    table->setCellWidget(index - 1, 3, maxLevelFrame);
    table->setCellWidget(index - 1, 4, percentLabel);

    // TODO: lock max spinbox to min spinbox
}

QTableWidget *MonTabWidget::tableAt(int tabIndex) {
    return static_cast<QTableWidget *>(this->widget(tabIndex));
}





//
void MonTabWidget::setTabActive(int index, bool active) {
    activeTabs[index] = active;
    setTabEnabled(index, active);
}
