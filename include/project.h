#ifndef PROJECT_H
#define PROJECT_H

#include "map.h"
#include "blockdata.h"
#include "heallocation.h"
#include "event.h"
#include "wildmoninfo.h"
#include "parseutil.h"

#include <QStringList>
#include <QList>
#include <QVector>
#include <QPair>
#include <QStandardItem>

static QString NONE_MAP_CONSTANT = "MAP_NONE";
static QString NONE_MAP_NAME = "None";

class Project
{
public:
    Project();
    QString root;
    QStringList *groupNames = nullptr;
    QMap<QString, int> *map_groups;
    QList<QStringList> groupedMapNames;
    QStringList *mapNames = nullptr;
    QList<HealLocation> flyableMaps;
    QMap<QString, QString>* mapConstantsToMapNames;
    QMap<QString, QString>* mapNamesToMapConstants;
    QList<QString> mapLayoutsTable;
    QList<QString> mapLayoutsTableMaster;
    QString layoutsLabel;
    QMap<QString, MapLayout*> mapLayouts;
    QMap<QString, MapLayout*> mapLayoutsMaster;
    QMap<QString, QString> *mapSecToMapHoverName;
    QMap<QString, int> mapSectionNameToValue;
    QMap<int, QString> mapSectionValueToName;
    QStringList *itemNames = nullptr;
    QStringList *flagNames = nullptr;
    QStringList *varNames = nullptr;
    QStringList *movementTypes = nullptr;
    QStringList *mapTypes = nullptr;
    QStringList *mapBattleScenes = nullptr;
    QStringList *weatherNames = nullptr;
    QStringList *coordEventWeatherNames = nullptr;
    QStringList *secretBaseIds = nullptr;
    QStringList *bgEventFacingDirections = nullptr;
    QMap<QString, int> metatileBehaviorMap;
    QMap<int, QString> metatileBehaviorMapInverse;
    QMap<QString, QString> facingDirections;
    ParseUtil parser;

    void set_root(QString);

    struct DataQualifiers
    {
        bool isStatic;
        bool isConst;
    };
    DataQualifiers getDataQualifiers(QString, QString);
    QMap<QString, DataQualifiers> dataQualifiers;

    QMap<QString, Map*> *map_cache;
    Map* loadMap(QString);
    Map* getMap(QString);

    QMap<QString, Tileset*> *tileset_cache = nullptr;
    Tileset* loadTileset(QString, Tileset *tileset = nullptr);
    Tileset* getTileset(QString, bool forceLoad = false);

    Blockdata* readBlockdata(QString);
    void loadBlockdata(Map*);

    void saveTextFile(QString path, QString text);
    void appendTextFile(QString path, QString text);
    void deleteFile(QString path);

    void readMapGroups();
    Map* addNewMapToGroup(QString mapName, int groupNum);
    Map* addNewMapToGroup(QString, int, Map*, bool);
    QString getNewMapName();
    QString getProjectTitle();

    QString readMapLayoutId(QString map_name);
    QString readMapLocation(QString map_name);

    void readWildMonData();
    QMap<QString, QMap<QString, WildPokemonHeader>> wildMonData;// "MAP_CONSTANT": {base_label: wild_encounter_json
    QMap<QString, QString> encounterMapToBaseLabel;// delete this?
    QVector<QString> encounterGroupLabels;
    // when saving, preserve the extra fields for gBattlePyramidWildMonHeaders, gBattlePikeWildMonHeaders
    void readSpeciesIconPaths();
    QMap<QString, QString> speciesToIconPath;
    //QVector<QString> wildMonFields;
    QVector<QPair<QString, QVector<int>>> wildMonFields;
    // temporary?
    QMap<QString, QJsonObject> extraEncounterGroups;

    QMap<QString, bool> getTopLevelMapFields();
    bool loadMapData(Map*);
    void readMapLayouts();
    void loadMapLayout(Map*);
    void loadMapTilesets(Map*);
    void loadTilesetAssets(Tileset*);
    void loadTilesetTiles(Tileset*, QImage);
    void loadTilesetMetatiles(Tileset*);

    void saveLayoutBlockdata(Map*);
    void saveLayoutBorder(Map*);
    void writeBlockdata(QString, Blockdata*);
    void saveAllMaps();
    void saveMap(Map*);
    void saveAllDataStructures();
    void saveMapLayouts();
    void saveMapGroups();
    void saveWildMonData();
    void saveMapConstantsHeader();
    void saveHealLocationStruct(Map*);
    void saveTilesets(Tileset*, Tileset*);
    void saveTilesetMetatileAttributes(Tileset*);
    void saveTilesetMetatiles(Tileset*);
    void saveTilesetTilesImage(Tileset*);
    void saveTilesetPalettes(Tileset*, bool);

    QStringList getSongNames();
    QStringList getVisibilities();
    QMap<QString, QStringList> getTilesetLabels();
    void readTilesetProperties();
    void readRegionMapSections();
    void readItemNames();
    void readFlagNames();
    void readVarNames();
    void readMovementTypes();
    void readInitialFacingDirections();
    void readMapTypes();
    void readMapBattleScenes();
    void readWeatherNames();
    void readCoordEventWeatherNames();
    void readSecretBaseIds();
    void readBgEventFacingDirections();
    void readMetatileBehaviors();
    void readHealLocations();

    void loadEventPixmaps(QList<Event*> objects);
    QMap<QString, int> getEventObjGfxConstants();
    QString fixPalettePath(QString path);
    QString fixGraphicPath(QString path);

    void loadMapBorder(Map *map);

    void saveMapHealEvents(Map *map);

    static int getNumTilesPrimary();
    static int getNumTilesTotal();
    static int getNumMetatilesPrimary();
    static int getNumMetatilesTotal();
    static int getNumPalettesPrimary();
    static int getNumPalettesTotal();

private:
    void updateMapLayout(Map*);

    void setNewMapHeader(Map* map, int mapIndex);
    void setNewMapLayout(Map* map);
    void setNewMapBlockdata(Map* map);
    void setNewMapBorder(Map *map);
    void setNewMapEvents(Map *map);
    void setNewMapConnections(Map *map);

    static int num_tiles_primary;
    static int num_tiles_total;
    static int num_metatiles_primary;
    static int num_metatiles_total;
    static int num_pals_primary;
    static int num_pals_total;
};

#endif // PROJECT_H
