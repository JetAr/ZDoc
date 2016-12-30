//--------------------------------------------------------------------
// Microsoft OLE DB Test
//
// Copyright (C) 1995-2000 Microsoft Corporation
//
// @doc
//
// @module ErrorMsg.CPP | Template source file for all test modules.
//
// [issue]
// * IAccessor				done.
// * IAlterIndex			not implemented.
// * IAlterTable			not implemented.
// * IChapteredRowset		not implemented.
// * IColumnsInfo			done.
// * IColumnsRowset			done.
// * ICommand				done.
// * ICommandPersist		not implemented.
// * ICommandPrepare		done.
// * ICommandText			done.
// * ICommandWithParameters	done.
// * ICommandProperties		done.
// * IConvertType			done.
// * IDBAsynchStatus		not implemented.
// * IDBCreateCommand		done.
// * IDBCreateSession		done.
// * IDBDataSourceAdmin		not implemented.
// * IDBInfo				not implemented.
// * IDBInitialize			done.
// * IDBProperties			not implemented.
// * IDBSchemaRowset		not implemented.
// * IErrorInfo				not implemented.
// * IErrorLookup			not implemented.
// * IErrorRecords			not implemented.
// * IGetDataSource			not implemented.
// * IIndexDefinition		not implemented.
// * IMultipleResults		not implemented.
// * IOpenRowset			done.
// * IParentRowset			not implemented.
// * IRowset				done.
// * IRowsetChange			done.
// * IRowsetChapterMember	not implemented.
// * IRowsetFind			not implemented.
// * IRowsetIdentity		done.
// * IRowsetIndex			not implemented.
// * IRowsetInfo			done.
// * IRowsetLocate			done.
// * IRowsetRefresh			not implemented.
// * IRowsetResynch			not implemented.
// * IRowsetScroll			done.
// * IRowsetUpdate			done.
// * IRowsetView			not implemented.
// * ISequentialStream		not implemented.
// * ISessionProperties		not implemented.
// * ISourcesRowset			not implemented.
// * ISQLErrorInfo			not implemented.
// * ISupportErrorInfo		not implemented.
// * ITableDefinition		not implemented.
// * ITableDefinitionWithConstraint	not implemented.
// * ITransaction			not implemented.
// * ITransactionJoin		not implemented.
// * ITransactionLocal		done.
// * ITransactionObject		not implemented.
// * ITransactionOptions	not implemented.
// * IViewChapter			not implemented.
// * IViewFilter			not implemented.
// * IViewRowset			not implemented.
// * IViewSort				not implemented.
//
#include "modstandard.hpp"
#define  DBINITCONSTANTS	// Must be defined to initialize constants in OLEDB.H
#define  INITGUID
#include <msdaguid.h>
#include <oledb.h>
#include <oledberr.h>
#include "privlib.h"

#define SAFE_RELEASE_ACCESSOR(pIAcc, hAcc) {if ((pIAcc) && (hAcc) && \
	CHECK((pIAcc)->ReleaseAccessor((hAcc), NULL), S_OK)) (hAcc) = DB_NULL_HACCESSOR;}

inline void SAFE_RELEASE_ROW(IRowset *pIRowset, HROW hRow)
{
    if (pIRowset && hRow)
    {
        CHECK(pIRowset->ReleaseRows(1, &hRow, NULL, NULL, NULL), S_OK);
        hRow = NULL;
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Module Values
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// {{ TCW_MODULE_GLOBALS
DECLARE_MODULE_CLSID = { 0x6f034695, 0xf3cf, 0x11d3, { 0xa8, 0xc7, 0x00, 0xa0, 0xc9, 0x96, 0xe8, 0xe2 }};
DECLARE_MODULE_NAME("ErrorMsg");
DECLARE_MODULE_OWNER("takeshim");
DECLARE_MODULE_DESCRIP("Error Message Verification");
DECLARE_MODULE_VERSION(795921705);
// TCW_WizardVersion(2)
// TCW_Automation(True)
// }} TCW_MODULE_GLOBALS_END

BOOL VerifyMessage(HRESULT hrExp, HRESULT hrAct, IUnknown *pIUnknown, REFIID riid);

struct MESSAGE_ENTRY
{
    HRESULT hr;
    LPWSTR pwszDescription;
};
MESSAGE_ENTRY *g_rgMsgs;

// English
MESSAGE_ENTRY g_rgUSMsgs[] =
{
    0x80040E00, L"Accessor is invalid.",
    0x80040E01, L"Row could not be inserted into the rowset without exceeding provider's maximum number of active rows.",
    0x80040E02, L"Accessor is read-only. Operation failed.",
    0x80040E03, L"Values violate the database schema.",
    0x80040E04, L"Row handle is invalid.",
    0x80040E05, L"Object was open.",
    0x80040E06, L"Chapter is invalid.",
    0x80040E07, L"Data or literal value could not be converted to the type of the column in the data source, and the provider was unable to determine which columns could not be converted.  Data overflow or sign mismatch was not the cause.",
    0x80040E08, L"Binding information is invalid.",
    0x80040E09, L"Permission denied.",
    0x80040E0A, L"Column does not contain bookmarks or chapters.",
    0x80040E0B, L"Cost limits were rejected.",
    0x80040E0C, L"Command text was not set for the command object.",
    0x80040E0D, L"Query plan within the cost limit cannot be found.",
    0x80040E0E, L"Bookmark is invalid.",
    0x80040E0F, L"Lock mode is invalid.",
    0x80040E10, L"No value given for one or more required parameters.",
    0x80040E11, L"Column ID is invalid.",
    0x80040E12, L"Numerator was greater than denominator. Values must express ratio between zero and 1.",
    0x80040E13, L"Value is invalid.",
    0x80040E14, L"One or more errors occurred during processing of command.",
    0x80040E15, L"Command cannot be canceled.",
    0x80040E16, L"Command dialect is not supported by this provider.",
    0x80040E17, L"Data source object could not be created because the named data source already exists.",
    0x80040E18, L"Rowset position cannot be restarted.",
    0x80040E19, L"Object or data matching the name, range, or selection criteria was not found within the scope of this operation.",
    0x80040E1A, L"Provider has ownership of this tree.",
    0x80040E1B, L"Identity cannot be determined for newly inserted rows.",
    0x80040E1C, L"Goal was rejected because no nonzero weights were specified for any goals supported. Current goal was not changed.",
    0x80040E1D, L"Requested conversion is not supported.",
    0x80040E1E, L"No rows were returned because the offset value moves the position before the beginning or after the end of the rowset.",
    0x80040E1F, L"Information was requested for a query and the query was not set.",
    0x80040E20, L"Consumer's event handler called a non-reentrant method in the provider.",
    0x80040E21, L"Multiple-step OLE DB operation generated errors. Check each OLE DB status value, if available. No work was done.",
    0x80040E22, L"Non-NULL controlling IUnknown was specified, and either the requested interface was not \r\nIUnknown, or the provider does not support COM aggregation.",
    0x80040E23, L"Row handle referred to a deleted row or a row marked for deletion.",
    0x80040E24, L"Rowset does not support fetching backward.",
    0x80040E25, L"Row handles must all be released before new ones can be obtained.",
    0x80040E26, L"One or more storage flags are not supported.",
    0x80040E27, L"Comparison operator is invalid.",
    0x80040E28, L"Status flag was neither DBCOLUMNSTATUS_OK nor\r\nDBCOLUMNSTATUS_ISNULL.",
    0x80040E29, L"Rowset does not support scrolling backward.",
    0x80040E2A, L"Region handle is invalid.",
    0x80040E2B, L"Set of rows is not contiguous to, or does not overlap, the rows in the watch region.",
    0x80040E2C, L"Transition from ALL* to MOVE* or EXTEND* was specified.",
    0x80040E2D, L"Region is not a proper subregion of the region identified by the watch region handle.",
    0x80040E2E, L"Multiple-statement commands are not supported by this provider.",
    0x80040E2F, L"Value violated the integrity constraints for a column or table.",
    0x80040E30, L"Type name is invalid.",
    0x80040E31, L"Execution stopped because a resource limit was reached. No results were returned.",
    0x80040E32, L"Command object whose command tree contains a rowset or rowsets cannot be cloned.",
    0x80040E33, L"Current tree cannot be represented as text.",
    0x80040E34, L"Index already exists.",
    0x80040E35, L"Index does not exist.",
    0x80040E36, L"Index is in use.",
    0x80040E37, L"Table does not exist.",
    0x80040E38, L"Rowset used optimistic concurrency and the value of a column has changed since it was last read.",
    0x80040E39, L"Errors detected during the copy.",
    0x80040E3A, L"Precision is invalid.",
    0x80040E3B, L"Scale is invalid.",
    0x80040E3C, L"Table ID is invalid.",
    0x80040E3D, L"Type is invalid.",
    0x80040E3E, L"Column ID already exists or occurred more than once in the array of columns.",
    0x80040E3F, L"Table already exists.",
    0x80040E40, L"Table is in use.",
    0x80040E41, L"Locale ID is not supported.",
    0x80040E42, L"Record number is invalid.",
    0x80040E43, L"Form of bookmark is valid, but no row was found to match it.",
    0x80040E44, L"Property value is invalid.",
    0x80040E45, L"Rowset is not chaptered.",
    0x80040E46, L"One or more accessor flags were invalid.",
    0x80040E47, L"One or more storage flags are invalid.",
    0x80040E48, L"Reference accessors are not supported by this provider.",
    0x80040E49, L"Null accessors are not supported by this provider.",
    0x80040E4A, L"Command was not prepared.",
    0x80040E4B, L"Accessor is not a parameter accessor.",
    0x80040E4C, L"Accessor is write-only.",
    0x80040E4D, L"Authentication failed.",
    0x80040E4E, L"Operation was canceled.",
    0x80040E4F, L"Rowset is single-chaptered. The chapter was not released.",
    0x80040E50, L"Source handle is invalid.",
    0x80040E51, L"Provider cannot derive parameter information and SetParameterInfo has not been called.",
    0x80040E52, L"Data source object is already initialized.",
    0x80040E53, L"Method is not supported by this provider.",
    0x80040E54, L"Number of rows with pending changes exceeded the limit.",
    0x80040E55, L"Column does not exist.",
    0x80040E56, L"Pending changes exist on a row with a reference count of zero.",
    0x80040E57, L"Literal value in the command exceeded the range of the type of the associated column.",
    0x80040E58, L"HRESULT is invalid.",
    0x80040E59, L"Lookup ID is invalid.",
    0x80040E5A, L"DynamicError ID is invalid.",
    0x80040E5B, L"Most recent data for a newly inserted row could not be retrieved because the insert is pending.",
    0x80040E5C, L"Conversion flag is invalid.",
    0x80040E5D, L"Parameter name is unrecognized.",
    0x80040E5E, L"Multiple storage objects cannot be open simultaneously.",
    0x80040E5F, L"Filter cannot be opened.",
    0x80040E60, L"Order cannot be opened.",
    0x80040E61, L"Tuple is invalid.",
    0x80040E62, L"Coordinate is invalid.",
    0x80040E63, L"Axis is invalid.",
    0x80040E64, L"One or more cell ordinals is invalid.",
    0x80040E65, L"Column ID is invalid.",
    0x80040E67, L"Command does not have a DBID.",
    0x80040E68, L"DBID already exists.",
    0x80040E69, L"Session cannot be created because maximum number of active sessions was already reached. Consumer must release one or more sessions before creating a new session object. ",
    0x80040E6A, L"Trustee is invalid.",
    0x80040E6B, L"Trustee was not recognized for this data source.",
    0x80040E6C, L"Trustee does not support memberships or collections.",
    0x80040E6D, L"Object is invalid or unknown to the provider.",
    0x80040E6E, L"Object does not have an owner.",
    0x80040E6F, L"Access entry list is invalid.",
    0x80040E70, L"Trustee supplied as owner is invalid or unknown to the provider.",
    0x80040E71, L"Permission in the access entry list is invalid.",
    0x80040E72, L"Index ID is invalid.",
    0x80040E73, L"Format of the initialization string does not conform to the OLE DB specification.",
    0x80040E74, L"No OLE DB providers of this source type are registered.",
    0x80040E75, L"Initialization string specifies a provider that does not match the active provider.",
    0x80040E76, L"DBID is invalid.",
    0x80040E77, L"ConstraintType is invalid or not supported by the provider.",
    0x80040E78, L"ConstraintType is not DBCONSTRAINTTYPE_FOREIGNKEY and cForeignKeyColumns is not zero.",
    0x80040E79, L"Specified deferrability flag is invalid or not supported by the provider.",
    0x80040E80, L"MatchType is invalid or the value is not supported by the provider.",
    0x80040E8A, L"Constraint update rule or delete rule is invalid.",
    0x80040E8B, L"Constraint ID is invalid.",
    0x80040E8C, L"Command persistence flag is invalid.",
    0x80040E8D, L"rguidColumnType points to a GUID that does not match the object type of this column, or this column was not set.",
    0x80040E8E, L"URL is outside of scope.",
    0x80040E90, L"Column or constraint could not be dropped because it is referenced by a dependent view or constraint.",
    0x80040E91, L"Source row does not exist.",
    0x80040E92, L"OLE DB object represented by this URL is locked by one or more other processes.",
    0x80040E93, L"Client requested an object type that is valid only for a collection. ",
    0x80040E94, L"Caller requested write access to a read-only object.",
    0x80040E95, L"Asynchronous binding is not supported by this provider.",
    0x80040E96, L"Connection to the server for this URL cannot be established.",
    0x80040E97, L"Timeout occurred when attempting to bind to the object.",
    0x80040E98, L"Object cannot be created at this URL because an object named by this URL already exists.",
    0x80040E99, L"Constraint already exists.",
    0x80040E9A, L"Object cannot be created at this URL because the server is out of physical storage.",
    0x80040E9B, L"Safety settings on this computer prohibit accessing a data source on another domain.",
    0x80040EFF, L"Dummy error - need this error so that mc puts the above defines\r\ninside the FACILITY_WINDOWS guard, instead of leaving it empty",
    0x8004D001, L"Retaining abort is not supported or a new unit of work cannot be created.",
    0x8004D002, L"Transaction failed to commit for an unknown reason.",
    0x8004D008, L"Neither the isolation level nor a strengthening of it is supported.",
    0x8004D00A, L"New transaction cannot enlist in the specified transaction coordinator. ",
    0x8004D00B, L"Semantics of retention of isolation are not supported.",
    0x8004D00E, L"No transaction is active.",
    0x8004D00F, L"Operation is not supported.",
    0x8004D013, L"Cannot start more transactions on this session.",
    0x8004D016, L"Transaction status is in doubt.  A communication failure occurred, or a \r\ntransaction manager or resource manager failed.",
    0x8004D017, L"Time-outs are not supported.",
    0x8004D018, L"Commit or abort already in progress. Call was ignored.",
    0x8004D019, L"Transaction aborted before commit was called.",
    0x8004D01A, L"Cannot begin a new transaction because the log file is full.",
    0x8004D01B, L"Cannot connect to the transaction manager or the transaction \r\nmanager is unavailable.",
    0x8004D01C, L"Connection to the transaction manager failed.",
    0x8004D01D, L"Cannot create new transaction because capacity was exceeded.",
    0x8004D100, L"Transaction manager did not accept a connection request.",
    0x40EC0, L"Fetching requested number of rows will exceed total number of active rows supported by the rowset.",
    0x40EC1, L"One or more column types are incompatible. Conversion errors will occur during copying.",
    0x40EC2, L"Parameter type information was overridden by caller.",
    0x40EC3, L"Bookmark was skipped for deleted or nonmember row.",
    0x40EC5, L"No more rowsets.",
    0x40EC6, L"Start or end of rowset or chapter was reached.",
    0x40EC7, L"Command was reexecuted.",
    0x40EC8, L"Operation succeeded, but status array or string buffer could not be allocated. ",
    0x40EC9, L"No more results.",
    0x40ECA, L"Server cannot release or downgrade a lock until the end of the transaction.",
    0x40ECB, L"Weight is not supported or exceeded the supported limit, and was set to 0 or the supported limit.",
    0x40ECC, L"Consumer does not want to receive further notification calls for this operation. ",
    0x40ECD, L"Input dialect was ignored and command was processed using default dialect.",
    0x40ECE, L"Consumer does not want to receive further notification calls for this phase.",
    0x40ECF, L"Consumer does not want to receive further notification calls for this reason.",
    0x40ED0, L"Operation is being processed asynchronously.",
    0x40ED1, L"Command was executed to reposition to the start of the rowset. Either the order of the columns changed, or columns were added to or removed from the rowset.",
    0x40ED2, L"Method had some errors, which were returned in the error array.",
    0x40ED3, L"Row handle is invalid.",
    0x40ED4, L"Row handle referred to a deleted row.",
    0x40ED5, L"Provider cannot keep track of all the changes. Client must refetch the data associated with the watch region by using another method.",
    0x40ED6, L"Execution stopped because a resource limit was reached. Results obtained so far were returned, but execution cannot resume.",
    0x40ED7, L"Row object was requested on a non-singleton result. First row was returned.",
    0x40ED8, L"Lock was upgraded from the value specified.",
    0x40ED9, L"One or more properties were changed as allowed by provider.",
    0x40EDA, L"Multiple-step operation completed with one or more errors. Check each status value.",
    0x40EDB, L"Parameter is invalid.",
    0x40EDC, L"Updating a row caused more than one row to be updated in the data source.",
    0x40EDD, L"Row has no row-specific columns.",
    0x4D000, L"Asynchronous abort started.",
    0x4D008, L"Abort already in progress. Call was ignored.",
    0, NULL
};

// German message entries
MESSAGE_ENTRY g_rgGerMsgs[] =
{
    265920, L"Durch das Abrufen der angeforderten Anzahl von Zeilen wird die Gesamtanzahl von aktiven Zeilen, die von diesem Rowset unterst黷zt werden, 黚erstiegen.",
    265921, L"Mindestens ein Spaltentyp ist nicht kompatibel. W鋒rend des Kopierens werden Konvertierungsfehler auftreten.",
    265922, L"Informationen zum Parametertyp wurden vom Aufrufer 黚erschrieben.",
    265923, L"Das Lesezeichen f黵 eine gel鰏chte Zeile oder Nicht-Member-Zeile wurde ausgelassen.",
    265925, L"Keine weiteren Rowsets.",
    265926, L"Anfang oder Ende des Rowsets oder des Kapitels wurde erreicht.",
    265927, L"Der Befehl wurde erneut ausgef黨rt.",
    265928, L"Der Vorgang war erfolgreich, aber der Statusarray oder Zeichenfolgenpuffer konnte nicht zugewiesen werden. ",
    265929, L"Es gibt keine weiteren Ergebnisse.",
    265930, L"Freigabe oder Downgrade einer Sperrung durch den Server ist erst bei Beenden der Transaktion m鰃lich.",
    265931, L"Das angegebene Gewicht wurde nicht unterst黷zt oder 黚erstieg die unterst黷zte Begrenzung und wurde auf 0 oder die unterst黷zte Begrenzung gesetzt.",
    265932, L"Der Consumer m鯿hte f黵 diesen Vorgang keine weiteren Benachrichtigungen erhalten. ",
    265933, L"Der Eingabedialekt wurde ignoriert, und der Befehl wurde ausgef黨rt, indem der Standarddialekt verwendet wurde.",
    265934, L"Der Consumer m鯿hte f黵 diese Phase keine weiteren Benachrichtigungen erhalten.",
    265935, L"Der Consumer m鯿hte aus diesem Grund keine weiteren Benachrichtigungen erhalten.",
    265936, L"Die Operation wird asynchron verarbeitet.",
    265937, L"Der Befehl wurde ausgef黨rt, um an den Anfang des Rowsets zu gelangen. Dazu musste entweder die Reihenfolge der Spalten ge鋘dert werden, oder im Rowset mussten Spalten hinzugef黦t bzw. entfernt werden.",
    265938, L"Fehler bei diesem Verfahren. Die Fehler wurden im Fehlerarray wiedergegeben.",
    265939, L"Das Zeilenhandle ist ung黮tig.",
    265940, L"Das Zeilenhandle verwies auf eine gel鰏chte Zeile.",
    265941, L"Der Provider konnte nicht alle 膎derungen aufzeichnen. Der Client muss die mit dem 躡erwachungsbereich verkn黳ften Daten mit einem anderen Verfahren abrufen.",
    265942, L"Die Ausf黨rung wurde abgebrochen, da eine Ressourcenbegrenzung erreicht wurde. Bisher erhaltene Ergebnisse wurden zur點kgegeben, die Ausf黨rung kann jedoch nicht wiederaufgenommen werden.",
    265943, L"Das Zeilenobjekt wurde als Non-Singleton-Ergebnis angefordert. Die erste Zeile wurde zur點kgegeben.",
    265944, L"Eine Sperrung wurde aktualisiert.",
    265945, L"Es wurde mindestens eine Eigenschaft mehr ge鋘dert, wie vom Provider zugelassen.",
    265946, L"Eine aus mehreren Schritten bestehende Operation wurde mit mindestens einem Fehler abgeschlossen. Pr黤en Sie die einzelnen Statuswerte.",
    265947, L"Der Parameter ist ung黮tig.",
    265948, L"Durch die Aktualisierung dieser Zeile wurde mindestens eine Zeile in der Datenquelle aktualisiert.",
    265949, L"Zeile hat keine zeilenspezifischen Spalten.",
    315392, L"Asynchroner Abbruch gestartet.",
    315400, L"Abbruch wird bereits ausgef黨rt. Dieser Aufruf wurde ignoriert.",
    -2147217920, L"Ung黮tige Zugriffsberechtigung.",
    -2147217919, L"Eine Zeile kann nicht in das Rowset eingef黦t werden, ohne die von diesem Rowset unterst黷zte Anzahl aktiver Zeilen zu 黚ersteigen.",
    -2147217918, L"Die Zugriffsberechtigung ist schreibgesch黷zt. Der Vorgang konnte nicht ausgef黨rt werden.",
    -2147217917, L"Die Werte verletzen das Datenbankschema.",
    -2147217916, L"Das Zeilenhandle ist ung黮tig.",
    -2147217915, L"Das Objekt war ge鰂fnet.",
    -2147217914, L"Das Kapitel ist ung黮tig.",
    -2147217913, L"Die Daten oder der Literalwert konnten nicht in den Typ der Spalten in der Datenquelle konvertiert werden, und der Provider kann nicht feststellen, welche Spalten nicht konvertiert werden konnten. Die Ursache ist weder Daten黚erlauf noch Zeichen, die nicht 黚ereinstimmen .",
    -2147217912, L"Die Bindungsinformationen sind ung黮tig.",
    -2147217911, L"Zugriff verweigert.",
    -2147217910, L"Spalte enth鋖t keine Lesezeichen oder Kapitel.",
    -2147217909, L"Kostenbeschr鋘kungen wurden zur點kgewiesen.",
    -2147217908, L"F黵 das Befehlsobjekt wurde kein Befehlstext festgelegt.",
    -2147217907, L"Innerhalb der Kostenbeschr鋘kung wurde kein Abfrageplan gefunden.",
    -2147217906, L"Das Lesezeichen ist ung黮tig.",
    -2147217905, L"Der Sperrmodus ist ung黮tig.",
    -2147217904, L"F黵 mindestens einen erforderlichen Parameter wurde kein Wert angegeben.",
    -2147217903, L"Die Spalten-ID ist ung黮tig.",
    -2147217902, L"Der Z鋒ler war gr鲞er als der Nenner.  Werte m黶sen ein Verh鋖tnis zwischen Null und 1 ausdr點ken.",
    -2147217901, L"Der Wert ist ung黮tig.",
    -2147217900, L"Der Befehl enthielt mindestens einen Fehler",
    -2147217899, L"Der Befehl kann nicht abgebrochen werden.",
    -2147217898, L"Der Befehlsdialekt wird von dem Provider nicht unterst黷zt.",
    -2147217897, L"Das Datenquellenobjekt konnte nicht erstellt werden, da eine Datenquelle mit dem angegebenen Namen bereits existiert.",
    -2147217896, L"Die Position des Rowsets kann nicht neu gestartet werden.",
    -2147217895, L"Ein Objekt oder Daten, die den Angaben f黵 Namen, Bereich oder Auswahlkriterium entsprechen, konnten innerhalb dieses Vorganges nicht gefunden werden.",
    -2147217894, L"Der Provider ist Eigent黰er dieser Verzeichnisstruktur.",
    -2147217893, L"Die Identit鋞 der neu eingef黦ten Zeilen kann nicht festgestellt werden.",
    -2147217892, L"Das Ziel wurde zur點kgewiesen, da keine Gewichte, die ungleich Null sind, f黵 unterst黷zte Ziele angegeben wurden. Das aktuelle Ziel wurde nicht ge鋘dert.",
    -2147217891, L"Die angeforderte Konvertierung wird nicht unterst黷zt.",
    -2147217890, L"Es wurden keine Zeilen zur點kgegeben, da der Offset-Wert die Position entweder vor den Anfang oder hinter das Ende des Rowsets schiebt.",
    -2147217889, L"F黵 eine Abfrage wurden Informationen angefordert,\r\naber die Abfrage wurde nicht festgelegt.",
    -2147217888, L"Die Ereignisbehandlung des Consumers rief eine nicht reentrante Methode beim Provider auf.",
    -2147217887, L"Fehler bei einer aus mehreren Schritten bestehenden OLE DB-Operation.  Pr黤en Sie die einzelnen OLE DB-Statuswerte, falls vorhanden. Daten wurden nicht verarbeitet .",
    -2147217886, L"Es wurde eine steuernde \"IUnknown\" ungleich NULL angegeben,\r\naber entweder war die angeforderte Schnittstelle nicht \"IUnknown\", oder der Provider unterst黷zt keine COM Aggregation.",
    -2147217885, L"Das Zeilenhandle bezog sich auf eine gel鰏chte oder zum l鰏chen markierte Zeile.",
    -2147217884, L"Das Rowset kann nicht r點kw鋜ts abgerufen werden.",
    -2147217883, L"Alle Zeilenhandles m黶sen freigegeben werden, bevor neue abgerufen werden k鰊nen.",
    -2147217882, L"Mindestens ein Speicherattribut wird nicht unterst黷zt.",
    -2147217881, L"Der Vergleichsoperator ist ung黮tig.",
    -2147217880, L"Das Statusattribut war weder DBCOLUMNSTATUS_OK noch\r\nDBCOLUMNSTATUS_ISNULL.",
    -2147217879, L"Im Rowset kann nicht r點kw鋜ts gebl鋞tert werden.",
    -2147217878, L"Das Bereichshandle ist ung黮tig.",
    -2147217877, L"Das Rowset grenzt nicht an die Reihen im angegebenen 躡erwachungsbereich an und 黚erlappt auch nicht.",
    -2147217876, L"Es wurde ein 躡ergang von ALL* zu MOVE* oder EXTEND* angegeben.",
    -2147217875, L"Der Bereich ist kein zul鋝siger Teilbereich des vom Bereichshandle identifizierten  Bereiches.",
    -2147217874, L"Aus mehreren Anweisungen bestehende Befehle werden vom Provider nicht unterst黷zt.",
    -2147217873, L"Der Wert verletzte die Integrit鋞seinschr鋘kung einer Spalte oder Tabelle.",
    -2147217872, L"Der Typname ist ung黮tig.",
    -2147217871, L"Die Ausf黨rung wurde aufgrund von erreichter Ressourcenbegrenzung abgebrochen. Es wurden keine Ergebnisse zur點kgegeben.",
    -2147217870, L"Ein Befehlsobjekt, dessen Befehlsstruktur einen oder mehrere Rowsets enth鋖t, kann nicht dupliziert werden.",
    -2147217869, L"Die Aktuelle Verzeichnisstruktur kann nicht als Text dargestellt werden.",
    -2147217868, L"Index ist bereits vorhanden.",
    -2147217867, L"Index ist nicht vorhanden.",
    -2147217866, L"Der Index wird verwendet.",
    -2147217865, L"Tabelle ist nicht vorhanden.",
    -2147217864, L"Das Rowset verwendete vollst鋘dige Parallelit鋞, und der Wert einer Spalte wurde seit dem letzten Lesen ge鋘dert.",
    -2147217863, L"W鋒rend des Kopierens wurden Fehler gefunden.",
    -2147217862, L"Die Genauigkeitsangabe ist ung黮tig.",
    -2147217861, L"Die Skalierung ist ung黮tig.",
    -2147217860, L"Die Tabellen-ID ist ung黮tig.",
    -2147217859, L"Die Typangabe ist ung黮tig.",
    -2147217858, L"Die Spalten-ID existiert bereits oder trat in dem Spaltenarray mehrmals auf.",
    -2147217857, L"Tabelle ist bereits vorhanden.",
    -2147217856, L"Die Tabelle wird verwendet.",
    -2147217855, L"Gebietsschema-ID wird nicht unterst黷zt.",
    -2147217854, L"Die Datensatznummer ist ung黮tig.",
    -2147217853, L"Das Lesezeichen ist g黮tig, aber es konnte keine passende Zeile gefunden werden.",
    -2147217852, L"Der Eigenschaftswert ist ung黮tig.",
    -2147217851, L"Rowset ist nicht in Kapitel unterteilt.",
    -2147217850, L"Mindestens eine Zugriffsberechtigung war ung黮tig.",
    -2147217849, L"Mindestens ein Speicherflag ist ung黮tig.",
    -2147217848, L"Referenz-Zugriffsberechtigungen werden vom Provider nicht unterst黷zt.",
    -2147217847, L"NULL-Zugriffsberechtigungen werden vom Provider nicht unterst黷zt.",
    -2147217846, L"Der Befehl wurde nicht vorbereitet.",
    -2147217845, L"Zugriffsberechtigung ist keine Parameterzugriffsberechtigung.",
    -2147217844, L"Mit der Zugriffsberechtigung darf nur geschrieben werden.",
    -2147217843, L"Fehler bei der Authentifizierung.",
    -2147217842, L"Der Vorgang wurde abgebrochen.",
    -2147217841, L"Rowset besteht aus einem einzelnen Kapitel, und das Kapitel war nicht freigegeben.",
    -2147217840, L"Das Quellhandle ist ung黮tig.",
    -2147217839, L"Der Provider kann keine Parameterinformationen ermitteln und SetParameterInfo ist nicht aufgerufen worden.",
    -2147217838, L"Das Datenquellen-Objekt wurde bereits initialisiert.",
    -2147217837, L"Das Verfahren wird von diesem Provider nicht unterst黷zt.",
    -2147217836, L"Die Anzahl der Zeilen mit anstehenden 膎derungen 黚erstieg die Begrenzung.",
    -2147217835, L"Spalte ist nicht vorhanden.",
    -2147217834, L"Es gibt anstehende 膎derungen an einer Zeile mit einer Referenzanzahl von Null.",
    -2147217833, L"Ein Literalwert im Befehl hat den Bereich des Typs der verkn黳ften Spalte 黚erschritten.",
    -2147217832, L"HRESULT ist ung黮tig.",
    -2147217831, L"Die Lookup-ID ist ung黮tig.",
    -2147217830, L"Die DynamicError-ID ist ung黮tig.",
    -2147217829, L"Letzte Daten f黵 eine neu eingef黦te Zeile konnten nicht abgerufen werden, da die Einf黦ung noch nicht aktualisiert wurde.",
    -2147217828, L"Das Konvertierungsattribut ist ung黮tig.",
    -2147217827, L"Der Parametername wird nicht erkannt.",
    -2147217826, L"Mehrere Speicherobjekte k鰊nen nicht gleichzeitig ge鰂fnet werden.",
    -2147217825, L"Der Filter kann nicht ge鰂fnet werden.",
    -2147217824, L"Der Auftrag kann nicht ge鰂fnet werden.",
    -2147217823, L"Das Tuple ist ung黮tig.",
    -2147217822, L"Die Koordinate ist ung黮tig.",
    -2147217821, L"Die Achse ist ung黮tig.",
    -2147217820, L"Mindestens eine der Zellordinalzahlen ist ung黮tig.",
    -2147217819, L"Die Spalten-ID ist ung黮tig.",
    -2147217817, L"DBID-Angabe ist ung黮tig.",
    -2147217816, L"DBID-Angabe ist bereits vorhanden.",
    -2147217815, L"Die Sitzung kann nicht erstellt werden, da die maximale Anzahl der vom Provider unterst黷zten Sitzungen bereits erreicht wurde. Der Consumer muss mindestens eine aktuelle Sitzung freigeben, bevor ein neues Sitzungsobjekt erhalten werden kann. ",
    -2147217814, L"Der Vertrauensnehmer ist ung黮tig.",
    -2147217813, L"Der Vertrauensnehmer ist f黵 diese Datenquelle nicht zul鋝sig.",
    -2147217812, L"Der Vertrauensnehmer unterst黷zt keine Mitgliedschaften oder Auflistungen.",
    -2147217811, L"Das Objekt ist ung黮tig oder dem Provider nicht bekannt.",
    -2147217810, L"Das Objekt hat keinen Besitzer.",
    -2147217809, L"Die Zugriffseingabeliste ist ung黮tig.",
    -2147217808, L"Der als Besitzer angegebene Vertrauensnehmer ist ung黮tig oder dem Provider nicht bekannt.",
    -2147217807, L"Eine in der Zugriffseingabeliste enthaltene Berechtigung ist ung黮tig.",
    -2147217806, L"Die Index-ID ist ung黮tig.",
    -2147217805, L"Das Format der Initialisierungszeichenfolge entspricht nicht den OLE DB-Angaben.",
    -2147217804, L"Es sind keine OLE DB-Provider f黵 diesen Quelltyp registriert.",
    -2147217803, L"Die Initialisierungszeichenfolge legt einen Provider fest, der nicht dem aktiven Provider entspricht.",
    -2147217802, L"DBID ist ung黮tig.",
    -2147217801, L"ConstraintType ist ung黮tig oder nicht vom Provider unterst黷zt.",
    -2147217800, L"Der ConstraintType ist nicht DBCONSTRAINTTYPE_FOREIGNKEY und cForeignKeyColumns ist ungleich Null.",
    -2147217799, L"Die angegebene Verschiebung (\"Deferrability\") ist ung黮tig oder wird vom Provider nicht unterst黷zt.",
    -2147217792, L"MatchType ist ung黮tig oder der Wert wird vom Provider nicht unterst黷zt.",
    -2147217782, L"Die beschr鋘kende Update-Regel oder L鰏ch-Regel ist ung黮tig.",
    -2147217781, L"Die Beschr鋘kung existiert nicht.",
    -2147217780, L"Die Kennzeichnung des Befehls als dauerhaft ist ung黮tig.",
    -2147217779, L"rguidColumnType zeigt zu einer GUID, die nicht zum Objekttyp dieser Spalte passt oder diese Spalte war nicht definiert.",
    -2147217778, L"Der URL ist au遝rhalb des Bereiches.",
    -2147217776, L"Die Spalte oder Beschr鋘kung konnte nicht gel鰏cht werden, da sie sich auf eine abh鋘gige Ansicht oder Beschr鋘kung bezieht.",
    -2147217775, L"Es gibt keine Zeile als Quelle.",
    -2147217774, L"Das OLE DB-Objekt, das durch diesen URL repr鋝entiert wird,  wird von mindestens einem Prozess gesperrt.",
    -2147217773, L"Client hat einen Objekttyp angefordert, der nur f黵 eine Auflistung g黮tig ist. ",
    -2147217772, L"Der Aufrufer forderte Schreibzugriff f黵 ein schreibgesch黷ztes Objekt.",
    -2147217771, L"Der Provider unterst黷zt keine asynchronen Bindungen.",
    -2147217770, L"Eine Verbindung mit dem Server kann f黵 diesen URL nicht hergestellt werden.",
    -2147217769, L"Timeout bei dem Versuch, eine Bindung mit dem Objekt herzustellen.",
    -2147217768, L"Ein Objekt kann unter diesem URL nicht erstellt werden, da es ein durch diesen URL benanntes Objekt bereits gibt.",
    -2147217767, L"Einschr鋘kung existiert bereits.",
    -2147217766, L"Ein Objekt kann unter diesem URL nicht erstellt werden, da der physische Speicher des Servers nicht ausreicht.",
    -2147217765, L"Die Sicherheitseinstellungen dieses Computers verhindern den Zugriff auf Datenquellen in einer anderen Dom鋘e.",
    -2147217665, L"躡ungsfehler - dieser Fehler ist erforderlich, damit die obigen Definitionen\r\nin den FACILITY_WINDOWS-Schutz 黚ernommen werden, und dieser nicht leer bleibt.",
    -2147168255, L"Beibehaltender Abbruch wird nicht unterst黷zt, oder es konnte keine neue Arbeitseinheit erstellt werden.",
    -2147168254, L"Aus unbekanntem Grund konnte kein Commit f黵 die Transaktion ausgef黨rt werden.",
    -2147168248, L"Weder die Isolationsebene noch eine Verst鋜kung derselben kann unterst黷zt werden.",
    -2147168246, L"Die neue Transaktion kann im angegebenen Transaktionskoordinator nicht eingetragen werden. ",
    -2147168245, L"Die Semantik des Isolierungsvorbehalts wird nicht unterst黷zt.",
    -2147168242, L"Keine der Transaktionen ist aktiv.",
    -2147168241, L"Die Operation wird nicht unterst黷zt.",
    -2147168237, L"Es k鰊nen keine weiteren Transaktionen in dieser Sitzung gestartet werden.",
    -2147168234, L"Der Status der Transaktion ist unklar. Fehler bei der 躡ertragung, beim Transaktions-Manager\r\noder beim Ressourcen-Manager.",
    -2147168233, L"Timeouts werden nicht unterst黷zt.",
    -2147168232, L"Es wird bereits eine Commit- oder Abbruch-Operation ausgef黨rt. Dieser Aufruf wurde ignoriert.",
    -2147168231, L"Die Transaktion wurde abgebrochen, bevor Commit aufgerufen wurde.",
    -2147168230, L"Es kann keine neue Transaktion begonnen werden, da die Protokolldatei voll ist.",
    -2147168229, L"Es kann keine Verbindung mit dem Transaktions-Manager hergestellt\r\nwerden, oder er ist nicht verf黦bar.",
    -2147168228, L"Fehler beim Verbinden mit dem Transaktions-Manager.",
    -2147168227, L"Es kann keine neue Transaktion erstellt werden, da die Kapazit鋞 ausgesch鰌ft ist.",
    -2147168000, L"Der Transaktions-Manager hat eine Verbindungsanforderung nicht akzeptiert.",
    0, NULL
};

// Japanese
MESSAGE_ENTRY g_rgJPMsgs[] =
{
    0x80040E00, L"\x30A2\x30AF\x30BB\x30C3\x30B5\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E01, L"\x884C\x30BB\x30C3\x30C8\x3067\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x308B\x30A2\x30AF\x30C6\x30A3\x30D6\x884C\x6570\x306E\x4E0A\x9650\x3092\x8D85\x3048\x305A\x306B\x884C\x3092\x8FFD\x52A0\x3059\x308B\x3068\x304C\x3067\x304D\x307E\x305B\x3093\x3002",
    0x80040E02, L"\x30A2\x30AF\x30BB\x30C3\x30B5\x306F\x8AAD\x307F\x53D6\x308A\x5C02\x7528\x3067\x3059\x3002\x64CD\x4F5C\x306B\x5931\x6557\x3057\x307E\x3057\x305F\x3002",
    0x80040E03, L"\x5024\x304C\x30C7\x30FC\x30BF\x30D9\x30FC\x30B9\x0020\x30B9\x30AD\x30FC\x30DE\x306B\x9055\x53CD\x3057\x3066\x3044\x307E\x3059\x3002",
    0x80040E04, L"\x884C\x30CF\x30F3\x30C9\x30EB\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E05, L"\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x304C\x958B\x3044\x3066\x3044\x307E\x3057\x305F\x3002",
    0x80040E06, L"\x30C1\x30E3\x30D7\x30BF\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E07, L"\x30C7\x30FC\x30BF\x307E\x305F\x306F\x30EA\x30C6\x30E9\x30EB\x5024\x3092\x30C7\x30FC\x30BF\x0020\x30BD\x30FC\x30B9\x5185\x306E\x5217\x578B\x306B\x5909\x63DB\x3067\x304D\x305A\x3001\x30D7\x30ED\x30D0\x30A4\x30C0\x306F\x3069\x306E\x5217\x304C\x5909\x63DB\x3055\x308C\x306A\x3044\x304B\x3092\x5224\x65AD\x3067\x304D\x307E\x305B\x3093\x3067\x3057\x305F\x3002\x30C7\x30FC\x30BF\x0020\x30AA\x30FC\x30D0\x30FC\x30D5\x30ED\x30FC\x307E\x305F\x306F\x30B5\x30A4\x30F3\x306E\x4E0D\x4E00\x81F4\x4EE5\x5916\x306E\x539F\x56E0\x3067\x3059\x3002",
    0x80040E08, L"\x7121\x52B9\x306A\x30D0\x30A4\x30F3\x30C7\x30A3\x30F3\x30B0\x60C5\x5831\x3067\x3059\x3002",
    0x80040E09, L"\x8A31\x53EF\x304C\x62D2\x5426\x3055\x308C\x307E\x3057\x305F\x3002",
    0x80040E0A, L"\x884C\x306B\x30D6\x30C3\x30AF\x30DE\x30FC\x30AF\x307E\x305F\x306F\x30C1\x30E3\x30D7\x30BF\x304C\x542B\x307E\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E0B, L"\x30B3\x30B9\x30C8\x5236\x9650\x304C\x62D2\x5426\x3055\x308C\x307E\x3057\x305F\x3002",
    0x80040E0C, L"\x0043\x006F\x006D\x006D\x0061\x006E\x0064\x0020\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x306E\x0020\x0043\x006F\x006D\x006D\x0061\x006E\x0064\x0020\x30C6\x30AD\x30B9\x30C8\x304C\x8A2D\x5B9A\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E0D, L"\x30B3\x30B9\x30C8\x5236\x9650\x5185\x306B\x30AF\x30A8\x30EA\x0020\x30D7\x30E9\x30F3\x304C\x898B\x3064\x304B\x308A\x307E\x305B\x3093\x3002",
    0x80040E0E, L"\x30D6\x30C3\x30AF\x30DE\x30FC\x30AF\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E0F, L"\x30ED\x30C3\x30AF\x0020\x30E2\x30FC\x30C9\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E10, L"\x0031\x0020\x3064\x4EE5\x4E0A\x306E\x5FC5\x8981\x306A\x30D1\x30E9\x30E1\x30FC\x30BF\x306E\x5024\x304C\x8A2D\x5B9A\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E11, L"\x5217\x0020\x0049\x0044\x0020\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E12, L"\x5206\x5B50\x304C\x5206\x6BCD\x3088\x308A\x5927\x304D\x304F\x306A\x3063\x3066\x3044\x307E\x3059\x3002\x5024\x306F\x0020\x0030\x0020\x3068\x0020\x0031\x0020\x306E\x9593\x306E\x6BD4\x7387\x3067\x3042\x3089\x308F\x3055\x308C\x3066\x3044\x306A\x3051\x308C\x3070\x306A\x308A\x307E\x305B\x3093\x3002",
    0x80040E13, L"\x5024\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E14, L"\x30B3\x30DE\x30F3\x30C9\x51E6\x7406\x4E2D\x306B\x0020\x0031\x0020\x3064\x4EE5\x4E0A\x306E\x30A8\x30E9\x30FC\x304C\x767A\x751F\x3057\x307E\x3057\x305F\x3002",
    0x80040E15, L"\x30B3\x30DE\x30F3\x30C9\x3092\x53D6\x308A\x6D88\x305B\x307E\x305B\x3093\x3002",
    0x80040E16, L"\x30B3\x30DE\x30F3\x30C9\x306E\x8A00\x8A9E\x306F\x3053\x306E\x30D7\x30ED\x30D0\x30A4\x30C0\x306B\x3088\x3063\x3066\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E17, L"\x540C\x3058\x540D\x524D\x306E\x30C7\x30FC\x30BF\x0020\x30BD\x30FC\x30B9\x304C\x65E2\x306B\x5B58\x5728\x3059\x308B\x305F\x3081\x3001\x30C7\x30FC\x30BF\x0020\x30BD\x30FC\x30B9\x0020\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x3092\x4F5C\x6210\x3067\x304D\x307E\x305B\x3093\x3067\x3057\x305F\x3002",
    0x80040E18, L"\x884C\x30BB\x30C3\x30C8\x306E\x4F4D\x7F6E\x306F\x518D\x958B\x3067\x304D\x307E\x305B\x3093\x3002",
    0x80040E19, L"\x3053\x306E\x64CD\x4F5C\x306E\x7BC4\x56F2\x306B\x306F\x3001\x540D\x524D\x3001\x7BC4\x56F2\x3042\x308B\x3044\x306F\x9078\x5B9A\x6761\x4EF6\x306B\x4E00\x81F4\x3059\x308B\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x304C\x3042\x308A\x307E\x305B\x3093\x3067\x3057\x305F\x3002",
    0x80040E1A, L"\x3053\x306E\x30C4\x30EA\x30FC\x306F\x30D7\x30ED\x30D0\x30A4\x30C0\x304C\x6240\x6709\x3057\x3066\x3044\x307E\x3059\x3002",
    0x80040E1B, L"\x65B0\x3057\x3044\x633F\x5165\x884C\x306E\x0020\x0049\x0044\x0020\x3092\x8B58\x5225\x3067\x304D\x307E\x305B\x3093\x3002",
    0x80040E1C, L"\x30B4\x30FC\x30EB\x306B\x6307\x5B9A\x3057\x305F\x975E\x30BC\x30ED\x578B\x306E\x30A6\x30A8\x30A4\x30C8\x304C\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x306A\x3044\x305F\x3081\x3001\x30B4\x30FC\x30EB\x306F\x62D2\x5426\x3055\x308C\x307E\x3057\x305F\x3002\x73FE\x5728\x306E\x30B4\x30FC\x30EB\x306F\x5909\x66F4\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E1D, L"\x8981\x6C42\x3055\x308C\x305F\x5909\x63DB\x306F\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E1E, L"\x30AA\x30D5\x30BB\x30C3\x30C8\x5024\x306F\x884C\x30BB\x30C3\x30C8\x306E\x958B\x59CB\x524D\x307E\x305F\x306F\x7D42\x4E86\x5F8C\x306B\x4F4D\x7F6E\x3092\x52D5\x304B\x3059\x305F\x3081\x3001\x884C\x306F\x8FD4\x3055\x308C\x307E\x305B\x3093\x3067\x3057\x305F\x3002",
    0x80040E1F, L"\x30AF\x30A8\x30EA\x306E\x60C5\x5831\x304C\x8981\x6C42\x3055\x308C\x307E\x3057\x305F\x3002\x30AF\x30A8\x30EA\x306F\x8A2D\x5B9A\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E20, L"\x30B3\x30F3\x30B7\x30E5\x30FC\x30DE\x306E\x30A4\x30D9\x30F3\x30C8\x0020\x30CF\x30F3\x30C9\x30E9\x306F\x30D7\x30ED\x30D0\x30A4\x30C0\x306E\x518D\x5165\x4E0D\x53EF\x80FD\x306A\x30E1\x30BD\x30C3\x30C9\x3092\x547C\x3073\x51FA\x3057\x307E\x3057\x305F\x3002",
    0x80040E21, L"\x591A\x6BB5\x968E\x306E\x0020\x004F\x004C\x0045\x0020\x0044\x0042\x0020\x306E\x64CD\x4F5C\x3067\x30A8\x30E9\x30FC\x304C\x767A\x751F\x3057\x307E\x3057\x305F\x3002\x5404\x0020\x004F\x004C\x0045\x0020\x0044\x0042\x0020\x306E\x72B6\x614B\x306E\x5024\x3092\x30C1\x30A7\x30C3\x30AF\x3057\x3066\x304F\x3060\x3055\x3044\x3002\x4F5C\x696D\x306F\x7D42\x4E86\x3057\x307E\x305B\x3093\x3067\x3057\x305F\x3002",
    0x80040E22, L"\x975E\x0020\x004E\x0055\x004C\x004C\x0020\x306E\x5236\x5FA1\x5074\x306E\x0020\x0049\x0055\x006E\x006B\x006E\x006F\x0077\x006E\x0020\x304C\x8A2D\x5B9A\x3055\x308C\x307E\x3057\x305F\x304C\x3001\x8981\x6C42\x3055\x308C\x305F\x30A4\x30F3\x30BF\x30FC\x30D5\x30A7\x30A4\x30B9\x306F\x0020\x0049\x0055\x006E\x006B\x006E\x006F\x0077\x006E\x3067\x306A\x3044\x304B\x3001\r\n\x307E\x305F\x306F\x30D7\x30ED\x30D0\x30A4\x30C0\x304C\x0020\x0043\x004F\x004D\x0020\x96C6\x6210\x3092\x30B5\x30DD\x30FC\x30C8\x3057\x3066\x3044\x306A\x3044\x304B\x3067\x3059\x3002",
    0x80040E23, L"\x884C\x30CF\x30F3\x30C9\x30EB\x306F\x524A\x9664\x3055\x308C\x305F\x884C\x3001\x307E\x305F\x306F\x524A\x9664\x306E\x305F\x3081\x306B\x30DE\x30FC\x30AF\x3055\x308C\x305F\x884C\x3092\x53C2\x7167\x3057\x307E\x3057\x305F\x3002",
    0x80040E24, L"\x884C\x30BB\x30C3\x30C8\x306F\x9006\x65B9\x5411\x30D5\x30A7\x30C3\x30C1\x3092\x30B5\x30DD\x30FC\x30C8\x3057\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E25, L"\x65B0\x898F\x53D6\x5F97\x3059\x308B\x524D\x306B\x3001\x3059\x3079\x3066\x306E\x884C\x30CF\x30F3\x30C9\x30EB\x3092\x89E3\x653E\x3059\x308B\x5FC5\x8981\x304C\x3042\x308A\x307E\x3059\x3002",
    0x80040E26, L"\x0031\x0020\x3064\x4EE5\x4E0A\x306E\x683C\x7D0D\x30D5\x30E9\x30B0\x304C\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E27, L"\x6BD4\x8F03\x6F14\x7B97\x5B50\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E28, L"\x72B6\x614B\x30D5\x30E9\x30B0\x306F\x0020\x0044\x0042\x0043\x004F\x004C\x0055\x004D\x004E\x0053\x0054\x0041\x0054\x0055\x0053\x005F\x004F\x004B\x0020\x307E\x305F\x306F\x0020\r\n\x0044\x0042\x0043\x004F\x004C\x0055\x004D\x004E\x0053\x0054\x0041\x0054\x0055\x0053\x005F\x0049\x0053\x004E\x0055\x004C\x004C\x0020\x306E\x3069\x3061\x3089\x3067\x3082\x3042\x308A\x307E\x305B\x3093\x3002",
    0x80040E29, L"\x884C\x30BB\x30C3\x30C8\x306F\x9006\x65B9\x5411\x306E\x30B9\x30AF\x30ED\x30FC\x30EB\x3092\x30B5\x30DD\x30FC\x30C8\x3057\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E2A, L"\x9818\x57DF\x30CF\x30F3\x30C9\x30EB\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E2B, L"\x884C\x30BB\x30C3\x30C8\x306F\x3001\x6307\x5B9A\x30A6\x30A9\x30C3\x30C1\x9818\x57DF\x5185\x306E\x884C\x306B\x9023\x7D9A\x3057\x3066\x3044\x305F\x308A\x3001\x91CD\x306A\x3063\x305F\x308A\x3057\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E2C, L"\x0041\x004C\x004C\x002A\x0020\x304B\x3089\x0020\x004D\x004F\x0056\x0045\x002A\x0020\x307E\x305F\x306F\x0020\x0045\x0058\x0054\x0045\x004E\x0044\x002A\x0020\x3078\x306E\x5909\x63DB\x304C\x6307\x5B9A\x3055\x308C\x307E\x3057\x305F\x3002",
    0x80040E2D, L"\x9818\x57DF\x306F\x3001\x30A6\x30A9\x30C3\x30C1\x9818\x57DF\x30CF\x30F3\x30C9\x30EB\x306B\x3088\x3063\x3066\x8B58\x5225\x3055\x308C\x308B\x9818\x57DF\x306B\x5BFE\x3059\x308B\x9069\x5207\x306A\x5185\x90E8\x9818\x57DF\x3067\x306F\x3042\x308A\x307E\x305B\x3093",
    0x80040E2E, L"\x30DE\x30EB\x30C1\x0020\x30B9\x30C6\x30FC\x30C8\x30E1\x30F3\x30C8\x0020\x30B3\x30DE\x30F3\x30C9\x306F\x3053\x306E\x30D7\x30ED\x30D0\x30A4\x30C0\x306B\x3088\x3063\x3066\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E2F, L"\x5024\x304C\x5217\x307E\x305F\x306F\x30C6\x30FC\x30D6\x30EB\x306E\x6574\x5408\x6027\x5236\x7D04\x306B\x9055\x53CD\x3057\x3066\x3044\x307E\x3059\x3002",
    0x80040E30, L"\x578B\x306E\x540D\x524D\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E31, L"\x30EA\x30BD\x30FC\x30B9\x306E\x4E0A\x9650\x306B\x9054\x3057\x305F\x305F\x3081\x5B9F\x884C\x306F\x4E2D\x6B62\x3055\x308C\x307E\x3057\x305F\x3002\x7D50\x679C\x306F\x8FD4\x3055\x308C\x307E\x305B\x3093\x3067\x3057\x305F\x3002",
    0x80040E32, L"\x30B3\x30DE\x30F3\x30C9\x0020\x30C4\x30EA\x30FC\x306B\x884C\x30BB\x30C3\x30C8\x3092\x542B\x3080\x0020\x0043\x006F\x006D\x006D\x0061\x006E\x0064\x0020\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x306E\x8907\x88FD\x3092\x4F5C\x6210\x3067\x304D\x307E\x305B\x3093\x3002",
    0x80040E33, L"\x73FE\x5728\x306E\x30C4\x30EA\x30FC\x3092\x30C6\x30AD\x30B9\x30C8\x3068\x3057\x3066\x8868\x793A\x3067\x304D\x307E\x305B\x3093\x3002",
    0x80040E34, L"\x30A4\x30F3\x30C7\x30C3\x30AF\x30B9\x306F\x65E2\x306B\x5B58\x5728\x3057\x307E\x3059\x3002",
    0x80040E35, L"\x30A4\x30F3\x30C7\x30C3\x30AF\x30B9\x306F\x5B58\x5728\x3057\x307E\x305B\x3093\x3002",
    0x80040E36, L"\x30A4\x30F3\x30C7\x30C3\x30AF\x30B9\x306F\x4F7F\x7528\x4E2D\x3067\x3059\x3002",
    0x80040E37, L"\x30C6\x30FC\x30D6\x30EB\x306F\x5B58\x5728\x3057\x307E\x305B\x3093\x3002",
    0x80040E38, L"\x884C\x30BB\x30C3\x30C8\x304C\x30AA\x30D7\x30C6\x30A3\x30DF\x30B9\x30C6\x30A3\x30C3\x30AF\x540C\x6642\x5171\x6709\x3092\x4F7F\x7528\x3057\x307E\x3057\x305F\x3002\x5217\x306E\x5024\x306F\x6700\x5F8C\x306B\x8AAD\x307F\x8FBC\x307E\x308C\x305F\x5F8C\x3067\x5909\x66F4\x3055\x308C\x307E\x3057\x305F\x3002",
    0x80040E39, L"\x30B3\x30D4\x30FC\x4E2D\x306B\x30A8\x30E9\x30FC\x304C\x691C\x51FA\x3055\x308C\x307E\x3057\x305F\x3002",
    0x80040E3A, L"\x6709\x52B9\x6841\x6570\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E3B, L"\x5C0F\x6570\x70B9\x90E8\x6841\x6570\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E3C, L"\x30C6\x30FC\x30D6\x30EB\x0020\x0049\x0044\x0020\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E3D, L"\x30BF\x30A4\x30D7\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E3E, L"\x5217\x0020\x0049\x0044\x0020\x304C\x65E2\x306B\x5B58\x5728\x3059\x308B\x304B\x3001\x914D\x5217\x5185\x3067\x0020\x0031\x0020\x5EA6\x4EE5\x4E0A\x767A\x751F\x3057\x307E\x3057\x305F\x3002",
    0x80040E3F, L"\x30C6\x30FC\x30D6\x30EB\x304C\x65E2\x306B\x5B58\x5728\x3057\x307E\x3059\x3002",
    0x80040E40, L"\x30C6\x30FC\x30D6\x30EB\x306F\x4F7F\x7528\x4E2D\x3067\x3059\x3002",
    0x80040E41, L"\x30ED\x30B1\x30FC\x30EB\x0020\x0049\x0044\x0020\x306F\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E42, L"\x30EC\x30B3\x30FC\x30C9\x756A\x53F7\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E43, L"\x30D6\x30C3\x30AF\x30DE\x30FC\x30AF\x306E\x5F62\x5F0F\x306F\x6709\x52B9\x3067\x3059\x304C\x3001\x4E00\x81F4\x3059\x308B\x884C\x304C\x898B\x3064\x304B\x308A\x307E\x305B\x3093\x3002",
    0x80040E44, L"\x30D7\x30ED\x30D1\x30C6\x30A3\x306E\x5024\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E45, L"\x884C\x30BB\x30C3\x30C8\x306B\x306F\x30C1\x30E3\x30D7\x30BF\x304C\x4F5C\x6210\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E46, L"\x0031\x0020\x3064\x4EE5\x4E0A\x306E\x30A2\x30AF\x30BB\x30C3\x30B5\x306E\x30D5\x30E9\x30B0\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E47, L"\x0031\x0020\x3064\x4EE5\x4E0A\x306E\x683C\x7D0D\x30D5\x30E9\x30B0\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E48, L"\x53C2\x7167\x306B\x3088\x308B\x30A2\x30AF\x30BB\x30C3\x30B5\x306F\x3053\x306E\x30D7\x30ED\x30D0\x30A4\x30C0\x3067\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E49, L"\x004E\x0075\x006C\x006C\x0020\x30A2\x30AF\x30BB\x30C3\x30B5\x306F\x3053\x306E\x30D7\x30ED\x30D0\x30A4\x30C0\x3067\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E4A, L"\x30B3\x30DE\x30F3\x30C9\x304C\x7528\x610F\x3055\x308C\x307E\x305B\x3093\x3067\x3057\x305F\x3002",
    0x80040E4B, L"\x30A2\x30AF\x30BB\x30C3\x30B5\x306F\x30D1\x30E9\x30E1\x30FC\x30BF\x0020\x30A2\x30AF\x30BB\x30C3\x30B5\x3067\x306F\x3042\x308A\x307E\x305B\x3093\x3002",
    0x80040E4C, L"\x30A2\x30AF\x30BB\x30C3\x30B5\x306F\x66F8\x304D\x8FBC\x307F\x5C02\x7528\x3067\x3059\x3002",
    0x80040E4D, L"\x8A8D\x8A3C\x306B\x5931\x6557\x3057\x307E\x3057\x305F\x3002",
    0x80040E4E, L"\x64CD\x4F5C\x306F\x30AD\x30E3\x30F3\x30BB\x30EB\x3055\x308C\x307E\x3057\x305F\x3002",
    0x80040E4F, L"\x884C\x30BB\x30C3\x30C8\x306F\x5358\x4E00\x30C1\x30E3\x30D7\x30BF\x3067\x3059\x3002\x30C1\x30E3\x30D7\x30BF\x304C\x89E3\x653E\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3067\x3057\x305F\x3002",
    0x80040E50, L"\x30BD\x30FC\x30B9\x0020\x30CF\x30F3\x30C9\x30EB\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E51, L"\x30D7\x30ED\x30D0\x30A4\x30C0\x304C\x30D1\x30E9\x30E1\x30FC\x30BF\x60C5\x5831\x3092\x5F97\x3089\x308C\x305A\x0020\x0053\x0065\x0074\x0050\x0061\x0072\x0061\x006D\x0065\x0074\x0065\x0072\x0049\x006E\x0066\x006F\x0020\x304C\x547C\x3073\x51FA\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E52, L"\x30C7\x30FC\x30BF\x0020\x30BD\x30FC\x30B9\x0020\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x306F\x65E2\x306B\x521D\x671F\x5316\x3055\x308C\x3066\x3044\x307E\x3059\x3002",
    0x80040E53, L"\x30E1\x30BD\x30C3\x30C9\x306F\x3053\x306E\x30D7\x30ED\x30D0\x30A4\x30C0\x3067\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E54, L"\x4FDD\x7559\x4E2D\x306E\x5909\x66F4\x3092\x6301\x3064\x884C\x306E\x6570\x304C\x3001\x8A2D\x5B9A\x3055\x308C\x305F\x4E0A\x9650\x3092\x8D85\x3048\x307E\x3057\x305F\x3002",
    0x80040E55, L"\x5217\x304C\x5B58\x5728\x3057\x307E\x305B\x3093\x3002",
    0x80040E56, L"\x30BC\x30ED\x306E\x53C2\x7167\x30AB\x30A6\x30F3\x30C8\x3092\x6301\x3064\x884C\x306B\x4FDD\x7559\x4E2D\x306E\x5909\x66F4\x304C\x3042\x308A\x307E\x3059\x3002",
    0x80040E57, L"\x30B3\x30DE\x30F3\x30C9\x306E\x30EA\x30C6\x30E9\x30EB\x5024\x304C\x3001\x95A2\x9023\x4ED8\x3051\x3089\x308C\x305F\x5217\x306E\x7A2E\x985E\x306E\x7BC4\x56F2\x3092\x8D85\x3048\x3066\x3044\x307E\x3059\x3002",
    0x80040E58, L"\x0048\x0052\x0045\x0053\x0055\x004C\x0054\x0020\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E59, L"\x004C\x006F\x006F\x006B\x0075\x0070\x0049\x0044\x0020\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E5A, L"\x0044\x0079\x006E\x0061\x006D\x0069\x0063\x0045\x0072\x0072\x006F\x0072\x0049\x0044\x0020\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E5B, L"\x633F\x5165\x304C\x4FDD\x7559\x4E2D\x306A\x306E\x3067\x3001\x65B0\x3057\x304F\x633F\x5165\x3055\x308C\x305F\x884C\x306E\x6700\x65B0\x306E\x30C7\x30FC\x30BF\x3092\x53D6\x5F97\x3067\x304D\x307E\x305B\x3093\x3067\x3057\x305F\x3002",
    0x80040E5C, L"\x5909\x63DB\x30D5\x30E9\x30B0\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E5D, L"\x30D1\x30E9\x30E1\x30FC\x30BF\x540D\x306F\x8A8D\x8B58\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E5E, L"\x8907\x6570\x306E\x683C\x7D0D\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x3092\x540C\x6642\x306B\x958B\x3051\x307E\x305B\x3093\x3002",
    0x80040E5F, L"\x30D5\x30A3\x30EB\x30BF\x3092\x958B\x3051\x307E\x305B\x3093\x3002",
    0x80040E60, L"\x547D\x4EE4\x3092\x958B\x3051\x307E\x305B\x3093\x3002",
    0x80040E61, L"\x7D44\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E62, L"\x5EA7\x6A19\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E63, L"\x8EF8\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E64, L"\x0031\x0020\x3064\x4EE5\x4E0A\x306E\x30BB\x30EB\x5E8F\x6570\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E65, L"\x0043\x006F\x006C\x0075\x006D\x006E\x0049\x0044\x0020\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E67, L"\x30B3\x30DE\x30F3\x30C9\x306B\x0020\x0044\x0042\x0049\x0044\x0020\x304C\x542B\x307E\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E68, L"\x0044\x0042\x0049\x0044\x0020\x306F\x65E2\x306B\x5B58\x5728\x3057\x307E\x3059\x3002",
    0x80040E69, L"\x30D7\x30ED\x30D0\x30A4\x30C0\x3067\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x308B\x6700\x5927\x0020\x0053\x0065\x0073\x0073\x0069\x006F\x006E\x0020\x6570\x306B\x9054\x3057\x305F\x305F\x3081\x3001\x0053\x0065\x0073\x0073\x0069\x006F\x006E\x0020\x3092\x4F5C\x6210\x3067\x304D\x307E\x305B\x3093\x3002\x65B0\x3057\x3044\x0020\x0053\x0065\x0073\x0073\x0069\x006F\x006E\x0020\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x3092\x4F5C\x6210\x3059\x308B\x524D\x306B\x3001\x30BB\x30C3\x30B7\x30E7\x30F3\x3092\x0020\x0031\x0020\x3064\x4EE5\x4E0A\x89E3\x653E\x3059\x308B\x5FC5\x8981\x304C\x3042\x308A\x307E\x3059\x3002\x0020",
    0x80040E6A, L"\x53D7\x8A17\x5024\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E6B, L"\x53D7\x8A17\x5024\x306F\x3001\x3053\x306E\x30C7\x30FC\x30BF\x0020\x30BD\x30FC\x30B9\x7528\x3067\x306F\x3042\x308A\x307E\x305B\x3093\x3002",
    0x80040E6C, L"\x53D7\x8A17\x5024\x306F\x3001\x30E1\x30F3\x30D0\x30FC\x30B7\x30C3\x30D7\x307E\x305F\x306F\x30B3\x30EC\x30AF\x30B7\x30E7\x30F3\x3092\x30B5\x30DD\x30FC\x30C8\x3057\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E6D, L"\x3053\x306E\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x306F\x3001\x30D7\x30ED\x30D0\x30A4\x30C0\x306B\x5BFE\x3057\x3066\x7121\x52B9\x307E\x305F\x306F\x4E0D\x660E\x3067\x3059\x3002",
    0x80040E6E, L"\x3053\x306E\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x306B\x306F\x6240\x6709\x8005\x304C\x3044\x307E\x305B\x3093\x3002",
    0x80040E6F, L"\x30A2\x30AF\x30BB\x30B9\x0020\x30A8\x30F3\x30C8\x30EA\x306E\x4E00\x89A7\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E70, L"\x6240\x6709\x8005\x3068\x3057\x3066\x4E0E\x3048\x3089\x308C\x305F\x53D7\x8AFE\x5024\x306F\x3001\x30D7\x30ED\x30D0\x30A4\x30C0\x306B\x5BFE\x3057\x3066\x7121\x52B9\x307E\x305F\x306F\x4E0D\x660E\x3067\x3059\x3002",
    0x80040E71, L"\x30A2\x30AF\x30BB\x30B9\x0020\x30A8\x30F3\x30C8\x30EA\x306E\x4E00\x89A7\x306B\x7121\x52B9\x306A\x8A31\x53EF\x304C\x3042\x308A\x307E\x3059\x3002",
    0x80040E72, L"\x30A4\x30F3\x30C7\x30C3\x30AF\x30B9\x0020\x0049\x0044\x0020\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E73, L"\x521D\x671F\x5316\x6587\x5B57\x5217\x306E\x5F62\x5F0F\x306F\x0020\x004F\x004C\x0045\x0020\x0044\x0042\x0020\x4ED5\x69D8\x306B\x9069\x5408\x3057\x307E\x305B\x3093\x3002",
    0x80040E74, L"\x3053\x306E\x30BD\x30FC\x30B9\x578B\x306E\x0020\x004F\x004C\x0045\x0020\x0044\x0042\x0020\x30D7\x30ED\x30D0\x30A4\x30C0\x306F\x767B\x9332\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E75, L"\x521D\x671F\x5316\x6587\x5B57\x5217\x306F\x3001\x30A2\x30AF\x30C6\x30A3\x30D6\x306A\x30D7\x30ED\x30D0\x30A4\x30C0\x306B\x4E00\x81F4\x3057\x306A\x3044\x30D7\x30ED\x30D0\x30A4\x30C0\x3092\x6307\x5B9A\x3057\x3066\x3044\x307E\x3059\x3002",
    0x80040E76, L"\x0044\x0042\x0049\x0044\x0020\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E77, L"\x0043\x006F\x006E\x0073\x0074\x0072\x0061\x0069\x006E\x0074\x0054\x0079\x0070\x0065\x0020\x306F\x7121\x52B9\x304B\x3001\x30D7\x30ED\x30D0\x30A4\x30C0\x304C\x30B5\x30DD\x30FC\x30C8\x3057\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E78, L"\x0043\x006F\x006E\x0073\x0074\x0072\x0061\x0069\x006E\x0074\x0054\x0079\x0070\x0065\x0020\x306F\x0020\x0044\x0042\x0043\x004F\x004E\x0053\x0054\x0052\x0041\x0049\x004E\x0054\x0054\x0059\x0050\x0045\x005F\x0046\x004F\x0052\x0045\x0049\x0047\x004E\x004B\x0045\x0059\x0020\x3067\x306F\x306A\x304F\x3001\x0063\x0046\x006F\x0072\x0065\x0069\x0067\x006E\x004B\x0065\x0079\x0043\x006F\x006C\x0075\x006D\x006E\x0073\x0020\x306F\x0020\x0030\x0020\x3067\x306F\x3042\x308A\x307E\x305B\x3093\x3002",
    0x80040E79, L"\x6307\x5B9A\x3055\x308C\x305F\x0020\x0044\x0065\x0066\x0065\x0072\x0072\x0061\x0062\x0069\x006C\x0069\x0074\x0079\x0020\x30D5\x30E9\x30B0\x306F\x7121\x52B9\x304B\x3001\x3053\x306E\x30D7\x30ED\x30D0\x30A4\x30C0\x306B\x3088\x3063\x3066\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E80, L"\x004D\x0061\x0074\x0063\x0068\x0054\x0079\x0070\x0065\x0020\x306F\x7121\x52B9\x304B\x3001\x30D7\x30ED\x30D0\x30A4\x30C0\x304C\x5024\x3092\x30B5\x30DD\x30FC\x30C8\x3057\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E8A, L"\x5236\x7D04\x306E\x30A2\x30C3\x30D7\x30C7\x30FC\x30C8\x307E\x305F\x306F\x524A\x9664\x306E\x898F\x5247\x306F\x7121\x52B9\x3067\x3059\x3002",
#ifdef MDAC25
    0x80040E8B, L"\x5236\x7D04\x304C\x5B58\x5728\x3057\x307E\x305B\x3093\x3002",
#else
    0x80040E8B, L"\x5236\x7D04\x0020\x0049\x0044\x0020\x304C\x7121\x52B9\x3067\x3059\x3002",
#endif
    0x80040E8C, L"\x0043\x006F\x006D\x006D\x0061\x006E\x0064\x0020\x6301\x7D9A\x30D5\x30E9\x30B0\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x80040E8D, L"\x0072\x0067\x0075\x0069\x0064\x0043\x006F\x006C\x0075\x006D\x006E\x0054\x0079\x0070\x0065\x0020\x304C\x3053\x306E\x5217\x306E\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x578B\x306B\x4E00\x81F4\x3057\x306A\x3044\x0020\x0047\x0055\x0049\x0044\x0020\x3092\x30DD\x30A4\x30F3\x30C8\x3057\x305F\x304B\x3001\x3053\x306E\x5217\x306F\x8A2D\x5B9A\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3067\x3057\x305F\x3002",
    0x80040E8E, L"\x0055\x0052\x004C\x0020\x304C\x7BC4\x56F2\x5916\x3067\x3059\x3002",
    0x80040E90, L"\x4F9D\x5B58\x3059\x308B\x8868\x793A\x307E\x305F\x306F\x5236\x7D04\x306B\x3088\x3063\x3066\x53C2\x7167\x3055\x308C\x3066\x3044\x308B\x305F\x3081\x3001\x5217\x307E\x305F\x306F\x5236\x7D04\x3092\x30C9\x30ED\x30C3\x30D7\x3067\x304D\x307E\x305B\x3093\x3067\x3057\x305F\x3002",
    0x80040E91, L"\x30BD\x30FC\x30B9\x884C\x304C\x3042\x308A\x307E\x305B\x3093\x3002",
    0x80040E92, L"\x3053\x306E\x0020\x0055\x0052\x004C\x0020\x306B\x30EA\x30F3\x30AF\x3055\x308C\x3066\x3044\x308B\x0020\x004F\x004C\x0045\x0020\x0044\x0042\x0020\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x306F\x3001\x0031\x0020\x3064\x4EE5\x4E0A\x306E\x30D7\x30ED\x30BB\x30B9\x306B\x3088\x308A\x30ED\x30C3\x30AF\x3055\x308C\x3066\x3044\x307E\x3059\x3002",
    0x80040E93, L"\x30AF\x30E9\x30A4\x30A2\x30F3\x30C8\x306F\x3001\x30B3\x30EC\x30AF\x30B7\x30E7\x30F3\x306B\x306E\x307F\x6709\x52B9\x306A\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x3092\x8981\x6C42\x3057\x307E\x3057\x305F\x3002",
    0x80040E94, L"\x547C\x3073\x51FA\x3057\x306F\x8AAD\x307F\x53D6\x308A\x5C02\x7528\x306E\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x306B\x5BFE\x3057\x3066\x66F8\x304D\x8FBC\x307F\x3092\x8981\x6C42\x3057\x307E\x3057\x305F\x3002",
    0x80040E95, L"\x975E\x540C\x671F\x7D50\x5408\x306F\x3053\x306E\x30D7\x30ED\x30D0\x30A4\x30C0\x306B\x3088\x3063\x3066\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x80040E96, L"\x3053\x306E\x0020\x0055\x0052\x004C\x0020\x306E\x30B5\x30FC\x30D0\x30FC\x306B\x63A5\x7D9A\x3067\x304D\x307E\x305B\x3093\x3002",
    0x80040E97, L"\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x3092\x30D0\x30A4\x30F3\x30C9\x3057\x3088\x3046\x3068\x3057\x3066\x30BF\x30A4\x30E0\x30A2\x30A6\x30C8\x304C\x767A\x751F\x3057\x307E\x3057\x305F\x3002",
    0x80040E98, L"\x3053\x306E\x0020\x0055\x0052\x004C\x0020\x304C\x540D\x524D\x3092\x4ED8\x3051\x305F\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x306F\x65E2\x306B\x5B58\x5728\x3059\x308B\x305F\x3081\x3001\x3053\x306E\x0020\x0055\x0052\x004C\x0020\x3067\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x3092\x4F5C\x6210\x3067\x304D\x307E\x305B\x3093\x3067\x3057\x305F\x3002",
    0x80040E99, L"\x5236\x7D04\x306F\x65E2\x306B\x5B58\x5728\x3057\x307E\x3059\x3002",
    0x80040E9A, L"\x30B5\x30FC\x30D0\x30FC\x306B\x7269\x7406\x7684\x8A18\x61B6\x9818\x57DF\x304C\x7121\x3044\x305F\x3081\x3001\x30D7\x30ED\x30D0\x30A4\x30C0\x306F\x3053\x306E\x0020\x0055\x0052\x004C\x0020\x3067\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x3092\x4F5C\x6210\x3067\x304D\x307E\x305B\x3093\x3002",
    0x80040E9B, L"\x3053\x306E\x30B3\x30F3\x30D4\x30E5\x30FC\x30BF\x306E\x5B89\x5168\x6027\x306F\x3001\x307B\x304B\x306E\x30C9\x30E1\x30A4\x30F3\x306E\x30C7\x30FC\x30BF\x0020\x30BD\x30FC\x30B9\x306B\x30A2\x30AF\x30BB\x30B9\x3067\x304D\x306A\x3044\x3088\x3046\x306B\x8A2D\x5B9A\x3055\x308C\x3066\x3044\x307E\x3059\x3002",
    0x80040EFF, L"\x30C0\x30DF\x30FC\x0020\x30A8\x30E9\x30FC\x0020\x002D\x0020\x3053\x306E\x30A8\x30E9\x30FC\x306F\x3001\x0046\x0041\x0043\x0049\x004C\x0049\x0054\x0059\x005F\x0057\x0049\x004E\x0044\x004F\x0057\x0053\x0020\x30AC\x30FC\x30C9\x3092\x7A7A\x306B\x305B\x305A\x4E0A\x8A18\x306E\x5B9A\x7FA9\x3092\r\n\x914D\x7F6E\x3059\x308B\x305F\x3081\x306B\x5FC5\x8981\x3067\x3059\x3002",
    0x8004D001, L"\x9023\x7D9A\x3057\x3066\x3044\x308B\x4E2D\x6B62\x306E\x5B9F\x884C\x304C\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x306A\x3044\x304B\x3001\x65B0\x3057\x3044\x4F5C\x696D\x30E6\x30CB\x30C3\x30C8\x304C\x4F5C\x6210\x3067\x304D\x307E\x305B\x3093\x3067\x3057\x305F\x3002",
    0x8004D002, L"\x539F\x56E0\x4E0D\x660E\x306E\x30A8\x30E9\x30FC\x304C\x767A\x751F\x3057\x305F\x305F\x3081\x3001\x30C8\x30E9\x30F3\x30B6\x30AF\x30B7\x30E7\x30F3\x306F\x30B3\x30DF\x30C3\x30C8\x306B\x5931\x6557\x3057\x307E\x3057\x305F\x3002",
    0x8004D008, L"\x5206\x96E2\x30EC\x30D9\x30EB\x304A\x3088\x3073\x305D\x306E\x5F37\x5316\x306F\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x8004D00A, L"\x6307\x5B9A\x3055\x308C\x305F\x30C8\x30E9\x30F3\x30B6\x30AF\x30B7\x30E7\x30F3\x0020\x30B3\x30FC\x30C7\x30A3\x30CD\x30FC\x30BF\x306B\x3001\x65B0\x898F\x30C8\x30E9\x30F3\x30B6\x30AF\x30B7\x30E7\x30F3\x3092\x30A8\x30F3\x30EA\x30B9\x30C8\x3067\x304D\x307E\x305B\x3093\x3067\x3057\x305F\x3002",
    0x8004D00B, L"\x5206\x96E2\x4FDD\x6709\x6642\x9593\x306E\x610F\x5473\x306F\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x8004D00E, L"\x30A2\x30AF\x30C6\x30A3\x30D6\x306A\x30C8\x30E9\x30F3\x30B6\x30AF\x30B7\x30E7\x30F3\x304C\x3042\x308A\x307E\x305B\x3093\x3002",
    0x8004D00F, L"\x6F14\x7B97\x306F\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x8004D013, L"\x3053\x306E\x30BB\x30C3\x30B7\x30E7\x30F3\x3067\x306F\x3001\x3053\x308C\x4EE5\x4E0A\x306E\x30C8\x30E9\x30F3\x30B6\x30AF\x30B7\x30E7\x30F3\x3092\x958B\x59CB\x3067\x304D\x307E\x305B\x3093\x3002",
    0x8004D016, L"\x30C8\x30E9\x30F3\x30B6\x30AF\x30B7\x30E7\x30F3\x306E\x72B6\x614B\x306B\x554F\x984C\x304C\x3042\x308B\x53EF\x80FD\x6027\x304C\x3042\x308A\x307E\x3059\x3002\x901A\x4FE1\x306E\x5207\x65AD\x304C\x3042\x3063\x305F\x304B\x3001\r\n\x30C8\x30E9\x30F3\x30B6\x30AF\x30B7\x30E7\x30F3\x0020\x30DE\x30CD\x30FC\x30B8\x30E3\x3042\x308B\x3044\x306F\x30EA\x30BD\x30FC\x30B9\x0020\x30DE\x30CD\x30FC\x30B8\x30E3\x304C\x5931\x6557\x3057\x305F\x304B\x3067\x3059\x3002",
    0x8004D017, L"\x30BF\x30A4\x30E0\x30A2\x30A6\x30C8\x306F\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x307E\x305B\x3093\x3002",
    0x8004D018, L"\x30B3\x30DF\x30C3\x30C8\x307E\x305F\x306F\x4E2D\x6B62\x64CD\x4F5C\x304C\x65E2\x306B\x9032\x884C\x4E2D\x306E\x305F\x3081\x3001\x3053\x306E\x547C\x3073\x51FA\x3057\x306F\x7121\x8996\x3055\x308C\x307E\x3057\x305F\x3002",
    0x8004D019, L"\x30B3\x30DF\x30C3\x30C8\x304C\x547C\x3073\x51FA\x3055\x308C\x308B\x524D\x306B\x30C8\x30E9\x30F3\x30B6\x30AF\x30B7\x30E7\x30F3\x304C\x4E2D\x6B62\x3055\x308C\x307E\x3057\x305F\x3002",
    0x8004D01A, L"\x30ED\x30B0\x0020\x30D5\x30A1\x30A4\x30EB\x304C\x3044\x3063\x3071\x3044\x3067\x65B0\x898F\x30C8\x30E9\x30F3\x30B6\x30AF\x30B7\x30E7\x30F3\x3092\x958B\x59CB\x3067\x304D\x307E\x305B\x3093\x3002",
    0x8004D01B, L"\x30C8\x30E9\x30F3\x30B6\x30AF\x30B7\x30E7\x30F3\x0020\x30DE\x30CD\x30FC\x30B8\x30E3\x306B\x63A5\x7D9A\x3067\x304D\x306A\x3044\x304B\x3001\x30C8\x30E9\x30F3\x30B6\x30AF\x30B7\x30E7\x30F3\x0020\x30DE\x30CD\x30FC\x30B8\x30E3\x304C\x4F7F\x7528\x3067\x304D\x307E\x305B\x3093\x3002",
    0x8004D01C, L"\x30C8\x30E9\x30F3\x30B6\x30AF\x30B7\x30E7\x30F3\x0020\x30DE\x30CD\x30FC\x30B8\x30E3\x3078\x306E\x63A5\x7D9A\x306B\x5931\x6557\x3057\x307E\x3057\x305F\x3002",
    0x8004D01D, L"\x5BB9\x91CF\x3092\x8D85\x3048\x3066\x3044\x305F\x305F\x3081\x3001\x65B0\x898F\x30C8\x30E9\x30F3\x30B6\x30AF\x30B7\x30E7\x30F3\x3092\x4F5C\x6210\x3067\x304D\x307E\x305B\x3093\x3002",
    0x8004D100, L"\x30C8\x30E9\x30F3\x30B6\x30AF\x30B7\x30E7\x30F3\x0020\x30DE\x30CD\x30FC\x30B8\x30E3\x306F\x63A5\x7D9A\x8981\x6C42\x3092\x53D7\x3051\x4ED8\x3051\x307E\x305B\x3093\x3067\x3057\x305F\x3002",
    0x40EC0, L"\x8981\x6C42\x3055\x308C\x305F\x884C\x6570\x3092\x30D5\x30A7\x30C3\x30C1\x3059\x308B\x3068\x3001\x884C\x30BB\x30C3\x30C8\x3067\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x308B\x30A2\x30AF\x30C6\x30A3\x30D6\x884C\x6570\x306E\x4E0A\x9650\x3092\x8D85\x3048\x307E\x3059\x3002",
    0x40EC1, L"\x0031\x0020\x5217\x307E\x305F\x306F\x8907\x6570\x306E\x5217\x306E\x578B\x306B\x4E92\x63DB\x6027\x304C\x3042\x308A\x307E\x305B\x3093\x3002\x30B3\x30D4\x30FC\x4E2D\x306B\x5909\x63DB\x30A8\x30E9\x30FC\x304C\x767A\x751F\x3057\x307E\x3059\x3002",
    0x40EC2, L"\x30D1\x30E9\x30E1\x30FC\x30BF\x578B\x306E\x60C5\x5831\x304C\x3001\x547C\x3073\x51FA\x3057\x306B\x3088\x308A\x4E0A\x66F8\x304D\x3055\x308C\x3066\x3044\x307E\x3059\x3002",
    0x40EC3, L"\x524A\x9664\x3055\x308C\x305F\x304B\x3001\x307E\x305F\x306F\x975E\x30E1\x30F3\x30D0\x578B\x306E\x884C\x306E\x30D6\x30C3\x30AF\x30DE\x30FC\x30AF\x306F\x7121\x8996\x3055\x308C\x307E\x3057\x305F\x3002",
    0x40EC5, L"\x884C\x30BB\x30C3\x30C8\x306F\x3053\x308C\x4EE5\x4E0A\x3042\x308A\x307E\x305B\x3093\x3002",
    0x40EC6, L"\x884C\x30BB\x30C3\x30C8\x307E\x305F\x306F\x30C1\x30E3\x30D7\x30BF\x306E\x521D\x3081\x307E\x305F\x306F\x7D42\x308F\x308A\x306B\x9054\x3057\x307E\x3057\x305F\x3002",
    0x40EC7, L"\x30B3\x30DE\x30F3\x30C9\x304C\x518D\x5EA6\x5B9F\x884C\x3055\x308C\x307E\x3057\x305F\x3002",
    0x40EC8, L"\x64CD\x4F5C\x306F\x6210\x529F\x3057\x307E\x3057\x305F\x304C\x3001\x72B6\x614B\x306E\x914D\x5217\x307E\x305F\x306F\x6587\x5B57\x5217\x30D0\x30C3\x30D5\x30A1\x306F\x5272\x308A\x5F53\x3066\x3089\x308C\x307E\x305B\x3093\x3067\x3057\x305F\x3002\x0020",
    0x40EC9, L"\x7D50\x679C\x306F\x3053\x308C\x4EE5\x4E0A\x3042\x308A\x307E\x305B\x3093\x3002",
    0x40ECA, L"\x30C8\x30E9\x30F3\x30B6\x30AF\x30B7\x30E7\x30F3\x304C\x7D42\x4E86\x3059\x308B\x307E\x3067\x3001\x30B5\x30FC\x30D0\x30FC\x306B\x3088\x308B\x30ED\x30C3\x30AF\x306E\x89E3\x653E\x307E\x305F\x306F\x30C0\x30A6\x30F3\x30B0\x30EC\x30FC\x30C9\x306F\x884C\x308F\x308C\x307E\x305B\x3093\x3002",
    0x40ECB, L"\x30A6\x30A8\x30A4\x30C8\x304C\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x306A\x3044\x304B\x3001\x307E\x305F\x306F\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x308B\x4E0A\x9650\x5024\x3092\x8D85\x3048\x3066\x3044\x308B\x305F\x3081\x3001\x0030\x0020\x307E\x305F\x306F\x30B5\x30DD\x30FC\x30C8\x3055\x308C\x3066\x3044\x308B\x4E0A\x9650\x5024\x306B\x5909\x66F4\x3055\x308C\x307E\x3057\x305F",
    0x40ECC, L"\x30B3\x30F3\x30B7\x30E5\x30FC\x30DE\x306F\x3053\x306E\x64CD\x4F5C\x306B\x3064\x3044\x3066\x306E\x4ECA\x5F8C\x306E\x901A\x77E5\x547C\x3073\x51FA\x3057\x306E\x53D7\x4FE1\x3092\x5FC5\x8981\x3068\x3057\x3066\x3044\x307E\x305B\x3093\x3002",
    0x40ECD, L"\x5165\x529B\x3055\x308C\x305F\x8A00\x8A9E\x304C\x7121\x8996\x3055\x308C\x3001\x30B3\x30DE\x30F3\x30C9\x306F\x65E2\x5B9A\x306E\x8A00\x8A9E\x3067\x51E6\x7406\x3055\x308C\x307E\x3057\x305F\x3002",
    0x40ECE, L"\x30B3\x30F3\x30B7\x30E5\x30FC\x30DE\x306F\x3053\x306E\x30D5\x30A7\x30FC\x30BA\x306B\x95A2\x3059\x308B\x4ECA\x5F8C\x306E\x901A\x77E5\x547C\x3073\x51FA\x3057\x306E\x53D7\x4FE1\x3092\x5FC5\x8981\x3068\x3057\x3066\x3044\x307E\x305B\x3093\x3002",
    0x40ECF, L"\x30B3\x30F3\x30B7\x30E5\x30FC\x30DE\x306F\x3053\x306E\x7406\x7531\x306B\x3064\x3044\x3066\x306E\x4ECA\x5F8C\x306E\x901A\x77E5\x547C\x3073\x51FA\x3057\x306E\x53D7\x4FE1\x3092\x5FC5\x8981\x3068\x3057\x3066\x3044\x307E\x305B\x3093\x3002",
    0x40ED0, L"\x975E\x540C\x671F\x7684\x306B\x64CD\x4F5C\x304C\x51E6\x7406\x3055\x308C\x3066\x3044\x307E\x3059\x3002",
    0x40ED1, L"\x884C\x30BB\x30C3\x30C8\x306E\x521D\x3081\x306B\x3082\x3046\x4E00\x5EA6\x79FB\x52D5\x3059\x308B\x305F\x3081\x306E\x30B3\x30DE\x30F3\x30C9\x304C\x5B9F\x884C\x3055\x308C\x307E\x3057\x305F\x3002\x5217\x306E\x9806\x5E8F\x304C\x5909\x66F4\x3055\x308C\x3066\x3044\x308B\x304B\x3001\x307E\x305F\x306F\x884C\x30BB\x30C3\x30C8\x3067\x5217\x304C\x8FFD\x52A0\x307E\x305F\x306F\x524A\x9664\x3055\x308C\x3066\x3044\x307E\x3059\x3002",
    0x40ED2, L"\x30E1\x30BD\x30C3\x30C9\x306B\x3044\x304F\x3064\x304B\x306E\x30A8\x30E9\x30FC\x304C\x3042\x308A\x3001\x30A8\x30E9\x30FC\x914D\x5217\x306B\x8FD4\x3055\x308C\x307E\x3057\x305F\x3002",
    0x40ED3, L"\x884C\x30CF\x30F3\x30C9\x30EB\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x40ED4, L"\x884C\x30CF\x30F3\x30C9\x30EB\x306F\x524A\x9664\x3055\x308C\x305F\x884C\x3092\x53C2\x7167\x3057\x3066\x3044\x307E\x3059\x3002",
    0x40ED5, L"\x30D7\x30ED\x30D0\x30A4\x30C0\x306F\x3059\x3079\x3066\x306E\x5909\x66F4\x3092\x8A18\x9332\x3067\x304D\x307E\x305B\x3093\x3067\x3057\x305F\x3002\x30AF\x30E9\x30A4\x30A2\x30F3\x30C8\x306F\x307B\x304B\x306E\x30E1\x30BD\x30C3\x30C9\x3092\x4F7F\x3063\x3066\x3001\x30A6\x30A9\x30C3\x30C1\x9818\x57DF\x306B\x95A2\x9023\x306E\x3042\x308B\x30E1\x30BD\x30C3\x30C9\x3092\x518D\x5EA6\x30D5\x30A7\x30C3\x30C1\x3059\x308B\x5FC5\x8981\x304C\x3042\x308A\x307E\x3059\x3002",
    0x40ED6, L"\x30EA\x30BD\x30FC\x30B9\x306E\x4E0A\x9650\x306B\x9054\x3057\x305F\x305F\x3081\x5B9F\x884C\x304C\x4E2D\x6B62\x3055\x308C\x307E\x3057\x305F\x3002\x3053\x308C\x307E\x3067\x306B\x5F97\x3089\x308C\x305F\x7D50\x679C\x304C\x8FD4\x3055\x308C\x307E\x3059\x304C\x3001\x0020\x5B9F\x884C\x3092\x518D\x958B\x3067\x304D\x307E\x305B\x3093\x3002",
    0x40ED7, L"\x975E\x0020\x0073\x0069\x006E\x0067\x006C\x0065\x0074\x006F\x006E\x0020\x306E\x7D50\x679C\x3067\x884C\x30AA\x30D6\x30B8\x30A7\x30AF\x30C8\x3092\x8981\x6C42\x3057\x307E\x3057\x305F\x3002\x6700\x521D\x306E\x884C\x304C\x8FD4\x3055\x308C\x307E\x3057\x305F\x3002",
    0x40ED8, L"\x30ED\x30C3\x30AF\x304C\x6307\x5B9A\x3055\x308C\x305F\x5024\x3088\x308A\x9AD8\x304F\x306A\x308A\x307E\x3057\x305F\x3002",
    0x40ED9, L"\x30D7\x30ED\x30D0\x30A4\x30C0\x306E\x8A31\x53EF\x306B\x5F93\x3044\x3001\x0031\x0020\x3064\x4EE5\x4E0A\x306E\x30D7\x30ED\x30D1\x30C6\x30A3\x304C\x5909\x66F4\x3055\x308C\x307E\x3057\x305F\x3002",
    0x40EDA, L"\x591A\x6BB5\x968E\x306E\x64CD\x4F5C\x306F\x30A8\x30E9\x30FC\x3092\x0020\x0031\x0020\x3064\x4EE5\x4E0A\x767A\x751F\x3057\x3066\x7D42\x4E86\x3057\x307E\x3057\x305F\x3002\x5404\x72B6\x614B\x306E\x5024\x3092\x30C1\x30A7\x30C3\x30AF\x3057\x3066\x304F\x3060\x3055\x3044\x3002",
    0x40EDB, L"\x30D1\x30E9\x30E1\x30FC\x30BF\x304C\x7121\x52B9\x3067\x3059\x3002",
    0x40EDC, L"\x884C\x304C\x66F4\x65B0\x3055\x308C\x305F\x305F\x3081\x3001\x30C7\x30FC\x30BF\x0020\x30BD\x30FC\x30B9\x5185\x306E\x0020\x0031\x0020\x884C\x4EE5\x4E0A\x306E\x884C\x304C\x66F4\x65B0\x3055\x308C\x307E\x3057\x305F\x3002",
    0x40EDD, L"\x884C\x306B\x884C\x56FA\x6709\x306E\x5217\x304C\x3042\x308A\x307E\x305B\x3093\x3002",
    0x4D000, L"\x975E\x540C\x671F\x306E\x4E2D\x6B62\x51E6\x7406\x304C\x958B\x59CB\x3055\x308C\x307E\x3057\x305F\x3002",
    0x4D008, L"\x4E2D\x6B62\x64CD\x4F5C\x304C\x65E2\x306B\x9032\x884C\x4E2D\x306E\x305F\x3081\x3001\x547C\x3073\x51FA\x3057\x306F\x7121\x8996\x3055\x308C\x307E\x3057\x305F\x3002",
    0, NULL
};

/*
MESSAGE_ENTRY g_rgJPMsgs[] = {
0x80040E00, L"傾僋僙僢僒偑柍岠偱偡丅",
0x80040E01, L"峴僙僢僩偱僒億乕僩偝傟偰偄傞傾僋僥傿僽峴悢偺忋尷傪挻偊偢偵峴傪捛壛偡傞偲偑偱偒傑偣傫丅",
0x80040E02, L"傾僋僙僢僒偼撉傒庢傝愱梡偱偡丅憖嶌偵幐攕偟傑偟偨丅",
0x80040E03, L"抣偑僨乕僞儀乕僗 僗僉乕儅偵堘斀偟偰偄傑偡丅",
0x80040E04, L"峴僴儞僪儖偑柍岠偱偡丅",
0x80040E05, L"僆僽僕僃僋僩偑奐偄偰偄傑偟偨丅",
0x80040E06, L"僠儍僾僞偑柍岠偱偡丅",
0x80040E07, L"僨乕僞傑偨偼儕僥儔儖抣傪僨乕僞 僜乕僗撪偺楍宆偵曄姺偱偒偢丄僾儘僶僀僟偼偳偺楍偑曄姺偝傟側偄偐傪敾抐偱偒傑偣傫偱偟偨丅僨乕僞 僆乕僶乕僼儘乕傑偨偼僒僀儞偺晄堦抳埲奜偺尨場偱偡丅",
0x80040E08, L"柍岠側僶僀儞僨傿儞僌忣曬偱偡丅",
0x80040E09, L"嫋壜偑嫅斲偝傟傑偟偨丅",
0x80040E0A, L"峴偵僽僢僋儅乕僋傑偨偼僠儍僾僞偑娷傑傟偰偄傑偣傫丅",
0x80040E0B, L"僐僗僩惂尷偑嫅斲偝傟傑偟偨丅",
0x80040E0C, L"Command 僆僽僕僃僋僩偺 Command 僥僉僗僩偑愝掕偝傟偰偄傑偣傫丅",
0x80040E0D, L"僐僗僩惂尷撪偵僋僄儕 僾儔儞偑尒偮偐傝傑偣傫丅",
0x80040E0E, L"僽僢僋儅乕僋偑柍岠偱偡丅",
0x80040E0F, L"儘僢僋 儌乕僪偑柍岠偱偡丅",
0x80040E10, L"1 偮埲忋偺昁梫側僷儔儊乕僞偺抣偑愝掕偝傟偰偄傑偣傫丅",
0x80040E11, L"楍 ID 偑柍岠偱偡丅",
0x80040E12, L"暘巕偑暘曣傛傝戝偒偔側偭偰偄傑偡丅抣偼 0 偲 1 偺娫偺斾棪偱偁傜傢偝傟偰偄側偗傟偽側傝傑偣傫丅",
0x80040E13, L"抣偑柍岠偱偡丅",
0x80040E14, L"僐儅儞僪張棟拞偵 1 偮埲忋偺僄儔乕偑敪惗偟傑偟偨丅",
0x80040E15, L"僐儅儞僪傪庢傝徚偣傑偣傫丅",
0x80040E16, L"僐儅儞僪偺尵岅偼偙偺僾儘僶僀僟偵傛偭偰僒億乕僩偝傟偰偄傑偣傫丅",
0x80040E17, L"摨偠柤慜偺僨乕僞 僜乕僗偑婛偵懚嵼偡傞偨傔丄僨乕僞 僜乕僗 僆僽僕僃僋僩傪嶌惉偱偒傑偣傫偱偟偨丅",
0x80040E18, L"峴僙僢僩偺埵抲偼嵞奐偱偒傑偣傫丅",
0x80040E19, L"偙偺憖嶌偺斖埻偵偼丄柤慜丄斖埻偁傞偄偼慖掕忦審偵堦抳偡傞僆僽僕僃僋僩偑偁傝傑偣傫偱偟偨丅",
0x80040E1A, L"偙偺僣儕乕偼僾儘僶僀僟偑強桳偟偰偄傑偡丅",
0x80040E1B, L"怴偟偄憓擖峴偺 ID 傪幆暿偱偒傑偣傫丅",
0x80040E1C, L"僑乕儖偵巜掕偟偨旕僛儘宆偺僂僄僀僩偑僒億乕僩偝傟偰偄側偄偨傔丄僑乕儖偼嫅斲偝傟傑偟偨丅尰嵼偺僑乕儖偼曄峏偝傟偰偄傑偣傫丅",
0x80040E1D, L"梫媮偝傟偨曄姺偼僒億乕僩偝傟偰偄傑偣傫丅",
0x80040E1E, L"僆僼僙僢僩抣偼峴僙僢僩偺奐巒慜傑偨偼廔椆屻偵埵抲傪摦偐偡偨傔丄峴偼曉偝傟傑偣傫偱偟偨丅",
0x80040E1F, L"僋僄儕偺忣曬偑梫媮偝傟傑偟偨丅僋僄儕偼愝掕偝傟偰偄傑偣傫丅",
0x80040E20, L"僐儞僔儏乕儅偺僀儀儞僩 僴儞僪儔偼僾儘僶僀僟偺嵞擖晄壜擻側儊僜僢僪傪屇傃弌偟傑偟偨丅",
0x80040E21, L"懡抜奒偺 OLE DB 偺憖嶌偱僄儔乕偑敪惗偟傑偟偨丅奺 OLE DB 偺忬懺偺抣傪僠僃僢僋偟偰偔偩偝偄丅嶌嬈偼廔椆偟傑偣傫偱偟偨丅",
0x80040E22, L"旕 NULL 偺惂屼懁偺 IUnknown 偑愝掕偝傟傑偟偨偑丄梫媮偝傟偨僀儞僞乕僼僃僀僗偼 IUnknown偱側偄偐丄\r\n傑偨偼僾儘僶僀僟偑 COM 廤惉傪僒億乕僩偟偰偄側偄偐偱偡丅",
0x80040E23, L"峴僴儞僪儖偼嶍彍偝傟偨峴丄傑偨偼嶍彍偺偨傔偵儅乕僋偝傟偨峴傪嶲徠偟傑偟偨丅",
0x80040E24, L"峴僙僢僩偼媡曽岦僼僃僢僠傪僒億乕僩偟偰偄傑偣傫丅",
0x80040E25, L"怴婯庢摼偡傞慜偵丄偡傋偰偺峴僴儞僪儖傪夝曻偡傞昁梫偑偁傝傑偡丅",
0x80040E26, L"1 偮埲忋偺奿擺僼儔僌偑僒億乕僩偝傟偰偄傑偣傫丅",
0x80040E27, L"斾妑墘嶼巕偑柍岠偱偡丅",
0x80040E28, L"忬懺僼儔僌偼 DBCOLUMNSTATUS_OK 傑偨偼 \r\nDBCOLUMNSTATUS_ISNULL 偺偳偪傜偱傕偁傝傑偣傫丅",
0x80040E29, L"峴僙僢僩偼媡曽岦偺僗僋儘乕儖傪僒億乕僩偟偰偄傑偣傫丅",
0x80040E2A, L"椞堟僴儞僪儖偑柍岠偱偡丅",
0x80040E2B, L"峴僙僢僩偼丄巜掕僂僅僢僠椞堟撪偺峴偵楢懕偟偰偄偨傝丄廳側偭偨傝偟偰偄傑偣傫丅",
0x80040E2C, L"ALL* 偐傜 MOVE* 傑偨偼 EXTEND* 傊偺曄姺偑巜掕偝傟傑偟偨丅",
0x80040E2D, L"椞堟偼丄僂僅僢僠椞堟僴儞僪儖偵傛偭偰幆暿偝傟傞椞堟偵懳偡傞揔愗側撪晹椞堟偱偼偁傝傑偣傫",
0x80040E2E, L"儅儖僠 僗僥乕僩儊儞僩 僐儅儞僪偼偙偺僾儘僶僀僟偵傛偭偰僒億乕僩偝傟偰偄傑偣傫丅",
0x80040E2F, L"抣偑楍傑偨偼僥乕僽儖偺惍崌惈惂栺偵堘斀偟偰偄傑偡丅",
0x80040E30, L"宆偺柤慜偑柍岠偱偡丅",
0x80040E31, L"儕僜乕僗偺忋尷偵払偟偨偨傔幚峴偼拞巭偝傟傑偟偨丅寢壥偼曉偝傟傑偣傫偱偟偨丅",
0x80040E32, L"僐儅儞僪 僣儕乕偵峴僙僢僩傪娷傓 Command 僆僽僕僃僋僩偺暋惢傪嶌惉偱偒傑偣傫丅",
0x80040E33, L"尰嵼偺僣儕乕傪僥僉僗僩偲偟偰昞帵偱偒傑偣傫丅",
0x80040E34, L"僀儞僨僢僋僗偼婛偵懚嵼偟傑偡丅",
0x80040E35, L"僀儞僨僢僋僗偼懚嵼偟傑偣傫丅",
0x80040E36, L"僀儞僨僢僋僗偼巊梡拞偱偡丅",
0x80040E37, L"僥乕僽儖偼懚嵼偟傑偣傫丅",
0x80040E38, L"峴僙僢僩偑僆僾僥傿儈僗僥傿僢僋摨帪嫟桳傪巊梡偟傑偟偨丅楍偺抣偼嵟屻偵撉傒崬傑傟偨屻偱曄峏偝傟傑偟偨丅",
0x80040E39, L"僐僺乕拞偵僄儔乕偑専弌偝傟傑偟偨丅",
0x80040E3A, L"桳岠寘悢偑柍岠偱偡丅",
0x80040E3B, L"彫悢揰晹寘悢偑柍岠偱偡丅",
0x80040E3C, L"僥乕僽儖 ID 偑柍岠偱偡丅",
0x80040E3D, L"僞僀僾偑柍岠偱偡丅",
0x80040E3E, L"楍 ID 偑婛偵懚嵼偡傞偐丄攝楍撪偱 1 搙埲忋敪惗偟傑偟偨丅",
0x80040E3F, L"僥乕僽儖偑婛偵懚嵼偟傑偡丅",
0x80040E40, L"僥乕僽儖偼巊梡拞偱偡丅",
0x80040E41, L"儘働乕儖 ID 偼僒億乕僩偝傟偰偄傑偣傫丅",
0x80040E42, L"儗僐乕僪斣崋偑柍岠偱偡丅",
0x80040E43, L"僽僢僋儅乕僋偺宍幃偼桳岠偱偡偑丄堦抳偡傞峴偑尒偮偐傝傑偣傫丅",
0x80040E44, L"僾儘僷僥傿偺抣偑柍岠偱偡丅",
0x80040E45, L"峴僙僢僩偵偼僠儍僾僞偑嶌惉偝傟偰偄傑偣傫丅",
0x80040E46, L"1 偮埲忋偺傾僋僙僢僒偺僼儔僌偑柍岠偱偡丅",
0x80040E47, L"1 偮埲忋偺奿擺僼儔僌偑柍岠偱偡丅",
0x80040E48, L"嶲徠偵傛傞傾僋僙僢僒偼偙偺僾儘僶僀僟偱僒億乕僩偝傟偰偄傑偣傫丅",
0x80040E49, L"Null 傾僋僙僢僒偼偙偺僾儘僶僀僟偱僒億乕僩偝傟偰偄傑偣傫丅",
0x80040E4A, L"僐儅儞僪偑梡堄偝傟傑偣傫偱偟偨丅",
0x80040E4B, L"傾僋僙僢僒偼僷儔儊乕僞 傾僋僙僢僒偱偼偁傝傑偣傫丅",
0x80040E4C, L"傾僋僙僢僒偼彂偒崬傒愱梡偱偡丅",
0x80040E4D, L"擣徹偵幐攕偟傑偟偨丅",
0x80040E4E, L"憖嶌偼僉儍儞僙儖偝傟傑偟偨丅",
0x80040E4F, L"峴僙僢僩偼扨堦僠儍僾僞偱偡丅僠儍僾僞偑夝曻偝傟偰偄傑偣傫偱偟偨丅",
0x80040E50, L"僜乕僗 僴儞僪儖偑柍岠偱偡丅",
0x80040E51, L"僾儘僶僀僟偑僷儔儊乕僞忣曬傪摼傜傟偢 SetParameterInfo 偑屇傃弌偝傟偰偄傑偣傫丅",
0x80040E52, L"僨乕僞 僜乕僗 僆僽僕僃僋僩偼婛偵弶婜壔偝傟偰偄傑偡丅",
0x80040E53, L"儊僜僢僪偼偙偺僾儘僶僀僟偱僒億乕僩偝傟偰偄傑偣傫丅",
0x80040E54, L"曐棷拞偺曄峏傪帩偮峴偺悢偑丄愝掕偝傟偨忋尷傪挻偊傑偟偨丅",
0x80040E55, L"楍偑懚嵼偟傑偣傫丅",
0x80040E56, L"僛儘偺嶲徠僇僂儞僩傪帩偮峴偵曐棷拞偺曄峏偑偁傝傑偡丅",
0x80040E57, L"僐儅儞僪偺儕僥儔儖抣偑丄娭楢晅偗傜傟偨楍偺庬椶偺斖埻傪挻偊偰偄傑偡丅",
0x80040E58, L"HRESULT 偑柍岠偱偡丅",
0x80040E59, L"LookupID 偑柍岠偱偡丅",
0x80040E5A, L"DynamicErrorID 偑柍岠偱偡丅",
0x80040E5B, L"憓擖偑曐棷拞側偺偱丄怴偟偔憓擖偝傟偨峴偺嵟怴偺僨乕僞傪庢摼偱偒傑偣傫偱偟偨丅",
0x80040E5C, L"曄姺僼儔僌偑柍岠偱偡丅",
0x80040E5D, L"僷儔儊乕僞柤偼擣幆偝傟偰偄傑偣傫丅",
0x80040E5E, L"暋悢偺奿擺僆僽僕僃僋僩傪摨帪偵奐偗傑偣傫丅",
0x80040E5F, L"僼傿儖僞傪奐偗傑偣傫丅",
0x80040E60, L"柦椷傪奐偗傑偣傫丅",
0x80040E61, L"慻偑柍岠偱偡丅",
0x80040E62, L"嵗昗偑柍岠偱偡丅",
0x80040E63, L"幉偑柍岠偱偡丅",
0x80040E64, L"1 偮埲忋偺僙儖彉悢偑柍岠偱偡丅",
0x80040E65, L"ColumnID 偑柍岠偱偡丅",
0x80040E67, L"僐儅儞僪偵 DBID 偑娷傑傟偰偄傑偣傫丅",
0x80040E68, L"DBID 偼婛偵懚嵼偟傑偡丅",
0x80040E69, L"僾儘僶僀僟偱僒億乕僩偝傟傞嵟戝 Session 悢偵払偟偨偨傔丄Session 傪嶌惉偱偒傑偣傫丅怴偟偄 Session 僆僽僕僃僋僩傪嶌惉偡傞慜偵丄僙僢僔儑儞傪 1 偮埲忋夝曻偡傞昁梫偑偁傝傑偡丅 ",
0x80040E6A, L"庴戸抣偑柍岠偱偡丅",
0x80040E6B, L"庴戸抣偼丄偙偺僨乕僞 僜乕僗梡偱偼偁傝傑偣傫丅",
0x80040E6C, L"庴戸抣偼丄儊儞僶乕僔僢僾傑偨偼僐儗僋僔儑儞傪僒億乕僩偟偰偄傑偣傫丅",
0x80040E6D, L"偙偺僆僽僕僃僋僩偼丄僾儘僶僀僟偵懳偟偰柍岠傑偨偼晄柧偱偡丅",
0x80040E6E, L"偙偺僆僽僕僃僋僩偵偼強桳幰偑偄傑偣傫丅",
0x80040E6F, L"傾僋僙僗 僄儞僩儕偺堦棗偑柍岠偱偡丅",
0x80040E70, L"強桳幰偲偟偰梌偊傜傟偨庴戻抣偼丄僾儘僶僀僟偵懳偟偰柍岠傑偨偼晄柧偱偡丅",
0x80040E71, L"傾僋僙僗 僄儞僩儕偺堦棗偵柍岠側嫋壜偑偁傝傑偡丅",
0x80040E72, L"僀儞僨僢僋僗 ID 偑柍岠偱偡丅",
0x80040E73, L"弶婜壔暥帤楍偺宍幃偼 OLE DB 巇條偵揔崌偟傑偣傫丅",
0x80040E74, L"偙偺僜乕僗宆偺 OLE DB 僾儘僶僀僟偼搊榐偝傟偰偄傑偣傫丅",
0x80040E75, L"弶婜壔暥帤楍偼丄傾僋僥傿僽側僾儘僶僀僟偵堦抳偟側偄僾儘僶僀僟傪巜掕偟偰偄傑偡丅",
0x80040E76, L"DBID 偑柍岠偱偡丅",
0x80040E77, L"ConstraintType 偼柍岠偐丄僾儘僶僀僟偑僒億乕僩偟偰偄傑偣傫丅",
0x80040E78, L"ConstraintType 偼 DBCONSTRAINTTYPE_FOREIGNKEY 偱偼側偔丄cForeignKeyColumns 偼 0 偱偼偁傝傑偣傫丅",
0x80040E79, L"巜掕偝傟偨 Deferrability 僼儔僌偼柍岠偐丄偙偺僾儘僶僀僟偵傛偭偰僒億乕僩偝傟偰偄傑偣傫丅",
0x80040E80, L"MatchType 偼柍岠偐丄僾儘僶僀僟偑抣傪僒億乕僩偟偰偄傑偣傫丅",
0x80040E8A, L"惂栺偺傾僢僾僨乕僩傑偨偼嶍彍偺婯懃偼柍岠偱偡丅",
#ifdef MDAC25
0x80040E8B, L"惂栺偑懚嵼偟傑偣傫丅",
#else
0x80040E8B, L"惂栺 ID 偑柍岠偱偡丅",
#endif
0x80040E8C, L"Command 帩懕僼儔僌偑柍岠偱偡丅",
0x80040E8D, L"rguidColumnType 偑偙偺楍偺僆僽僕僃僋僩宆偵堦抳偟側偄 GUID 傪億僀儞僩偟偨偐丄偙偺楍偼愝掕偝傟偰偄傑偣傫偱偟偨丅",
0x80040E8E, L"URL 偑斖埻奜偱偡丅",
0x80040E90, L"埶懚偡傞昞帵傑偨偼惂栺偵傛偭偰嶲徠偝傟偰偄傞偨傔丄楍傑偨偼惂栺傪僪儘僢僾偱偒傑偣傫偱偟偨丅",
0x80040E91, L"僜乕僗峴偑偁傝傑偣傫丅",
0x80040E92, L"偙偺 URL 偵儕儞僋偝傟偰偄傞 OLE DB 僆僽僕僃僋僩偼丄1 偮埲忋偺僾儘僙僗偵傛傝儘僢僋偝傟偰偄傑偡丅",
0x80040E93, L"僋儔僀傾儞僩偼丄僐儗僋僔儑儞偵偺傒桳岠側僆僽僕僃僋僩傪梫媮偟傑偟偨丅",
0x80040E94, L"屇傃弌偟偼撉傒庢傝愱梡偺僆僽僕僃僋僩偵懳偟偰彂偒崬傒傪梫媮偟傑偟偨丅",
0x80040E95, L"旕摨婜寢崌偼偙偺僾儘僶僀僟偵傛偭偰僒億乕僩偝傟偰偄傑偣傫丅",
0x80040E96, L"偙偺 URL 偺僒乕僶乕偵愙懕偱偒傑偣傫丅",
0x80040E97, L"僆僽僕僃僋僩傪僶僀儞僪偟傛偆偲偟偰僞僀儉傾僂僩偑敪惗偟傑偟偨丅",
0x80040E98, L"偙偺 URL 偑柤慜傪晅偗偨僆僽僕僃僋僩偼婛偵懚嵼偡傞偨傔丄偙偺 URL 偱僆僽僕僃僋僩傪嶌惉偱偒傑偣傫偱偟偨丅",
0x80040E99, L"惂栺偼婛偵懚嵼偟傑偡丅",
0x80040E9A, L"僒乕僶乕偵暔棟揑婰壇椞堟偑柍偄偨傔丄僾儘僶僀僟偼偙偺 URL 偱僆僽僕僃僋僩傪嶌惉偱偒傑偣傫丅",
0x80040E9B, L"偙偺僐儞僺儏乕僞偺埨慡惈偼丄傎偐偺僪儊僀儞偺僨乕僞 僜乕僗偵傾僋僙僗偱偒側偄傛偆偵愝掕偝傟偰偄傑偡丅",
0x80040EFF, L"僟儈乕 僄儔乕 - 偙偺僄儔乕偼丄FACILITY_WINDOWS 僈乕僪傪嬻偵偣偢忋婰偺掕媊傪\r\n攝抲偡傞偨傔偵昁梫偱偡丅",
0x8004D001, L"楢懕偟偰偄傞拞巭偺幚峴偑僒億乕僩偝傟偰偄側偄偐丄怴偟偄嶌嬈儐僯僢僩偑嶌惉偱偒傑偣傫偱偟偨丅",
0x8004D002, L"尨場晄柧偺僄儔乕偑敪惗偟偨偨傔丄僩儔儞僓僋僔儑儞偼僐儈僢僩偵幐攕偟傑偟偨丅",
0x8004D008, L"暘棧儗儀儖偍傛傃偦偺嫮壔偼僒億乕僩偝傟偰偄傑偣傫丅",
0x8004D00A, L"巜掕偝傟偨僩儔儞僓僋僔儑儞 僐乕僨傿僱乕僞偵丄怴婯僩儔儞僓僋僔儑儞傪僄儞儕僗僩偱偒傑偣傫偱偟偨丅",
0x8004D00B, L"暘棧曐桳帪娫偺堄枴偼僒億乕僩偝傟偰偄傑偣傫丅",
0x8004D00E, L"傾僋僥傿僽側僩儔儞僓僋僔儑儞偑偁傝傑偣傫丅",
0x8004D00F, L"墘嶼偼僒億乕僩偝傟偰偄傑偣傫丅",
0x8004D013, L"偙偺僙僢僔儑儞偱偼丄偙傟埲忋偺僩儔儞僓僋僔儑儞傪奐巒偱偒傑偣傫丅",
0x8004D016, L"僩儔儞僓僋僔儑儞偺忬懺偵栤戣偑偁傞壜擻惈偑偁傝傑偡丅捠怣偺愗抐偑偁偭偨偐丄\r\n僩儔儞僓僋僔儑儞 儅僱乕僕儍偁傞偄偼儕僜乕僗 儅僱乕僕儍偑幐攕偟偨偐偱偡丅",
0x8004D017, L"僞僀儉傾僂僩偼僒億乕僩偝傟偰偄傑偣傫丅",
0x8004D018, L"僐儈僢僩傑偨偼拞巭憖嶌偑婛偵恑峴拞偺偨傔丄偙偺屇傃弌偟偼柍帇偝傟傑偟偨丅",
0x8004D019, L"僐儈僢僩偑屇傃弌偝傟傞慜偵僩儔儞僓僋僔儑儞偑拞巭偝傟傑偟偨丅",
0x8004D01A, L"儘僌 僼傽僀儖偑偄偭傁偄偱怴婯僩儔儞僓僋僔儑儞傪奐巒偱偒傑偣傫丅",
0x8004D01B, L"僩儔儞僓僋僔儑儞 儅僱乕僕儍偵愙懕偱偒側偄偐丄僩儔儞僓僋僔儑儞 儅僱乕僕儍偑巊梡偱偒傑偣傫丅",
0x8004D01C, L"僩儔儞僓僋僔儑儞 儅僱乕僕儍傊偺愙懕偵幐攕偟傑偟偨丅",
0x8004D01D, L"梕検傪挻偊偰偄偨偨傔丄怴婯僩儔儞僓僋僔儑儞傪嶌惉偱偒傑偣傫丅",
0x8004D100, L"僩儔儞僓僋僔儑儞 儅僱乕僕儍偼愙懕梫媮傪庴偗晅偗傑偣傫偱偟偨丅",
0x40EC0, L"梫媮偝傟偨峴悢傪僼僃僢僠偡傞偲丄峴僙僢僩偱僒億乕僩偝傟偰偄傞傾僋僥傿僽峴悢偺忋尷傪挻偊傑偡丅",
0x40EC1, L"1 楍傑偨偼暋悢偺楍偺宆偵屳姺惈偑偁傝傑偣傫丅僐僺乕拞偵曄姺僄儔乕偑敪惗偟傑偡丅",
0x40EC2, L"僷儔儊乕僞宆偺忣曬偑丄屇傃弌偟偵傛傝忋彂偒偝傟偰偄傑偡丅",
0x40EC3, L"嶍彍偝傟偨偐丄傑偨偼旕儊儞僶宆偺峴偺僽僢僋儅乕僋偼柍帇偝傟傑偟偨丅",
0x40EC5, L"峴僙僢僩偼偙傟埲忋偁傝傑偣傫丅",
0x40EC6, L"峴僙僢僩傑偨偼僠儍僾僞偺弶傔傑偨偼廔傢傝偵払偟傑偟偨丅",
0x40EC7, L"僐儅儞僪偑嵞搙幚峴偝傟傑偟偨丅",
0x40EC8, L"憖嶌偼惉岟偟傑偟偨偑丄忬懺偺攝楍傑偨偼暥帤楍僶僢僼傽偼妱傝摉偰傜傟傑偣傫偱偟偨丅 ",
0x40EC9, L"寢壥偼偙傟埲忋偁傝傑偣傫丅",
0x40ECA, L"僩儔儞僓僋僔儑儞偑廔椆偡傞傑偱丄僒乕僶乕偵傛傞儘僢僋偺夝曻傑偨偼僟僂儞僌儗乕僪偼峴傢傟傑偣傫丅",
0x40ECB, L"僂僄僀僩偑僒億乕僩偝傟偰偄側偄偐丄傑偨偼僒億乕僩偝傟偰偄傞忋尷抣傪挻偊偰偄傞偨傔丄0 傑偨偼僒億乕僩偝傟偰偄傞忋尷抣偵曄峏偝傟傑偟偨",
0x40ECC, L"僐儞僔儏乕儅偼偙偺憖嶌偵偮偄偰偺崱屻偺捠抦屇傃弌偟偺庴怣傪昁梫偲偟偰偄傑偣傫丅",
0x40ECD, L"擖椡偝傟偨尵岅偑柍帇偝傟丄僐儅儞僪偼婛掕偺尵岅偱張棟偝傟傑偟偨丅",
0x40ECE, L"僐儞僔儏乕儅偼偙偺僼僃乕僘偵娭偡傞崱屻偺捠抦屇傃弌偟偺庴怣傪昁梫偲偟偰偄傑偣傫丅",
0x40ECF, L"僐儞僔儏乕儅偼偙偺棟桼偵偮偄偰偺崱屻偺捠抦屇傃弌偟偺庴怣傪昁梫偲偟偰偄傑偣傫丅",
0x40ED0, L"旕摨婜揑偵憖嶌偑張棟偝傟偰偄傑偡丅",
0x40ED1, L"峴僙僢僩偺弶傔偵傕偆堦搙堏摦偡傞偨傔偺僐儅儞僪偑幚峴偝傟傑偟偨丅楍偺弴彉偑曄峏偝傟偰偄傞偐丄傑偨偼峴僙僢僩偱楍偑捛壛傑偨偼嶍彍偝傟偰偄傑偡丅",
0x40ED2, L"儊僜僢僪偵偄偔偮偐偺僄儔乕偑偁傝丄僄儔乕攝楍偵曉偝傟傑偟偨丅",
0x40ED3, L"峴僴儞僪儖偑柍岠偱偡丅",
0x40ED4, L"峴僴儞僪儖偼嶍彍偝傟偨峴傪嶲徠偟偰偄傑偡丅",
0x40ED5, L"僾儘僶僀僟偼偡傋偰偺曄峏傪婰榐偱偒傑偣傫偱偟偨丅僋儔僀傾儞僩偼傎偐偺儊僜僢僪傪巊偭偰丄僂僅僢僠椞堟偵娭楢偺偁傞儊僜僢僪傪嵞搙僼僃僢僠偡傞昁梫偑偁傝傑偡丅",
0x40ED6, L"儕僜乕僗偺忋尷偵払偟偨偨傔幚峴偑拞巭偝傟傑偟偨丅偙傟傑偱偵摼傜傟偨寢壥偑曉偝傟傑偡偑丄 幚峴傪嵞奐偱偒傑偣傫丅",
0x40ED7, L"旕 singleton 偺寢壥偱峴僆僽僕僃僋僩傪梫媮偟傑偟偨丅嵟弶偺峴偑曉偝傟傑偟偨丅",
0x40ED8, L"儘僢僋偑巜掕偝傟偨抣傛傝崅偔側傝傑偟偨丅",
0x40ED9, L"僾儘僶僀僟偺嫋壜偵廬偄丄1 偮埲忋偺僾儘僷僥傿偑曄峏偝傟傑偟偨丅",
0x40EDA, L"懡抜奒偺憖嶌偼僄儔乕傪 1 偮埲忋敪惗偟偰廔椆偟傑偟偨丅奺忬懺偺抣傪僠僃僢僋偟偰偔偩偝偄丅",
0x40EDB, L"僷儔儊乕僞偑柍岠偱偡丅",
0x40EDC, L"峴偑峏怴偝傟偨偨傔丄僨乕僞 僜乕僗撪偺 1 峴埲忋偺峴偑峏怴偝傟傑偟偨丅",
0x40EDD, L"峴偵峴屌桳偺楍偑偁傝傑偣傫丅",
0x4D000, L"旕摨婜偺拞巭張棟偑奐巒偝傟傑偟偨丅",
0x4D008, L"拞巭憖嶌偑婛偵恑峴拞偺偨傔丄屇傃弌偟偼柍帇偝傟傑偟偨丅",
0, NULL };
*/

//--------------------------------------------------------------------
// @func Module level initialization routine
//
// @rdesc Success or Failure
// 		@flag  TRUE  | Successful initialization
//		@flag  FALSE | Initialization problems
//
// pThisTestModule->m_pIUnknown : IDBCreateSession
// pThisTestModule->m_pIUnknown2 : IUnknown of Session Object
// pThisTestModule->m_pVoid : CTable *
//
BOOL ModuleInit(CThisTestModule * pThisTestModule)
{
    OSVERSIONINFO osvi;
    LANGID langid;

    if(!ModuleCreateDBSession(pThisTestModule))
        return FALSE;

    pThisTestModule->m_pVoid = new CTable((IUnknown *)pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
    if (!CHECK(((CTable *)pThisTestModule->m_pVoid)->CreateTable(1), S_OK))
        return FALSE;

    // Obtain LangID
    memset(&osvi, 0, sizeof(osvi));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        if( osvi.dwMajorVersion >= 5)
        {
            // Windows 2000
            odtLog << L"Running in Windows 2000\n";
        }


        LANGID (WINAPI *pfnGetUserDefaultUILanguage)(void);
        HMODULE hKernel32 = LoadLibrary("KERNEL32.DLL");
        pfnGetUserDefaultUILanguage = (LANGID (WINAPI *)(void))::GetProcAddress(hKernel32,"GetUserDefaultUILanguage");
        FreeLibrary(hKernel32);
        langid = pfnGetUserDefaultUILanguage();
    }
    else
    {
        langid = GetUserDefaultLangID();
    }

    switch (PRIMARYLANGID(langid))
    {
    case LANG_ENGLISH:
        g_rgMsgs = g_rgUSMsgs;
        break;
    case LANG_JAPANESE:
        g_rgMsgs = g_rgJPMsgs;
        break;
    case LANG_GERMAN:
        g_rgMsgs = g_rgGerMsgs;
        break;
    default:
        odtLog << L"Unsupported LocaleID = " << GetUserDefaultLCID() << "\n";
        return FALSE;
    }

    return TRUE;
}

//--------------------------------------------------------------------
// @func Module level termination routine
//
// @rdesc Success or Failure
// 		@flag  TRUE  | Successful initialization
//		@flag  FALSE | Initialization problems
//
BOOL ModuleTerminate(CThisTestModule * pThisTestModule)
{
    if (pThisTestModule->m_pVoid)
    {
        ((CTable *)pThisTestModule->m_pVoid)->DropTable();
        delete (CTable*)pThisTestModule->m_pVoid;
        pThisTestModule->m_pVoid = NULL;
    }

    return ModuleReleaseDBSession(pThisTestModule);
}

/******************************************************************************
***
***
***
******************************************************************************/
int VerifyMessage(HRESULT hrExp, HRESULT hrAct, IUnknown *pIUnknown, REFIID riid)
{
    int nResult = TEST_FAIL;
    LPWSTR pwszExpDesc = L"";
    BSTR bstrActDesc = NULL;
    ISupportErrorInfo *pISupportErrorInfo = NULL;
    IErrorInfo *pIErrorInfo = NULL;
    IErrorInfo *pIErrorInfo2 = NULL;
    IErrorRecords *pIErrorRecords = NULL;
    HRESULT hr;

    // Get Expected Description
    for(MESSAGE_ENTRY *pMsg=g_rgMsgs; pMsg->pwszDescription; pMsg++)
    {
        if (pMsg->hr == hrExp)
        {
            pwszExpDesc = pMsg->pwszDescription;
            break;
        }
    }

    if (hrExp!= hrAct)
    {
        odtLog <<"EXPECTED:"<<GetErrorName(hrExp)<<"\n";
        odtLog <<"ACTUAL  :"<<GetErrorName(hrAct)<<"\n";
        nResult = TEST_FAIL;
        goto CLEANUP;
    }
//	odtLog << "EXPECT:" << GetErrorName(hrExp) << " " << pwszExpDesc << "\n";
    odtLog << "EXPECT:" << pwszExpDesc << "\n";

    // Get pIErrorInfo
    TESTC_(pIUnknown->QueryInterface(IID_ISupportErrorInfo, (void **)&pISupportErrorInfo), S_OK);
    TESTC_(pISupportErrorInfo->InterfaceSupportsErrorInfo(riid), S_OK);
    if ((hr = GetErrorInfo(0, &pIErrorInfo)) != S_OK)
    {
//		odtLog << "ACTUAL:" << GetErrorName(hrAct) << " (no error info)\n";
        odtLog << "ACTUAL:(no error info)\n";
        goto CLEANUP;
    }

    // Check ErrorRecords
    if ((hr = pIErrorInfo->QueryInterface(IID_IErrorRecords, (void **)&pIErrorRecords)) == S_OK)
    {
        ULONG cErrorRecords;
        pIErrorRecords->GetRecordCount(&cErrorRecords);
        for(ULONG i=0; i<cErrorRecords; i++)
        {
            ERRORINFO ErrorInfo;
            pIErrorRecords->GetBasicErrorInfo(i, &ErrorInfo);
            hr = pIErrorRecords->GetErrorInfo(i, GetUserDefaultLCID(), &pIErrorInfo2);
            if (hr == S_OK)
            {
                pIErrorInfo2->GetDescription(&bstrActDesc);
                if (bstrActDesc)
                {
//					odtLog << "ACTUAL:" << GetErrorName(hrAct) << " " << bstrActDesc << "\n";
                    odtLog << "ACTUAL:" << bstrActDesc << "\n";
//					if (wcscmp(bstrActDesc, pwszExpDesc) == 0) {
                    if (hrExp == ErrorInfo.hrError && wcscmp(bstrActDesc, pwszExpDesc) == 0)
                    {
                        nResult = TEST_PASS;
                    }
                    SysFreeString(bstrActDesc);
                }
                SAFE_RELEASE(pIErrorInfo2);
            }
        }
    }
    else
    {
        // Check ErrorInfo
        pIErrorInfo->GetDescription(&bstrActDesc);
        if (bstrActDesc)
        {
//			odtLog << "ACTUAL:" << GetErrorName(hrAct) << " " << bstrActDesc << "\n";
            odtLog << "ACTUAL:" << bstrActDesc << "\n";
            if (wcscmp(bstrActDesc, pwszExpDesc) == 0)
            {
//				nResult = TEST_PASS;
                nResult = TEST_CONFORMANCE_WARNING;
            }
            SysFreeString(bstrActDesc);
        }
    }

CLEANUP:
    SAFE_RELEASE(pIErrorRecords);
    SAFE_RELEASE(pIErrorInfo);
    SAFE_RELEASE(pISupportErrorInfo);
    return nResult;
}

HRESULT ExecuteCommand(ICommand *pICommand, LPWSTR wszCommandText, REFIID riid, IUnknown **ppIUnknown)
{
    HRESULT hr;
    ICommandText *pICommandText = NULL;

    if (!VerifyInterface(pICommand, IID_ICommandText, COMMAND_INTERFACE, (IUnknown**)&pICommandText))
    {
        hr = E_NOINTERFACE;
        goto CLEANUP;
    }

    if ((hr = pICommandText->SetCommandText(DBGUID_DBSQL, wszCommandText)) != S_OK)
        goto CLEANUP;

    hr = pICommand->Execute(NULL, riid, NULL, NULL, ppIUnknown);

CLEANUP:
    SAFE_RELEASE(pICommandText);
    return hr;
}


// {{ TCW_TEST_CASE_MAP(TCErrorMessages)
//*-----------------------------------------------------------------------
// @class
//
class TCErrorMessages : public CTestCases
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCErrorMessages,CTestCases);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember Verify Messages
    int Variation_1();
    // }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCErrorMessages)
#define THE_CLASS TCErrorMessages
BEG_TEST_CASE(TCErrorMessages, CTestCases, L"")
TEST_VARIATION(1, 		L"Verify Messages")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCAccessorErrors)
//*-----------------------------------------------------------------------
// @class IAccessor Error Messages
//
class TCAccessorErrors : public CCommandObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCAccessorErrors,CCommandObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_BADACCESSORHANDLE (80040E00)
    int Variation_1();
    // @cmember DB_E_BADACCESSORFLAGS (80040E46)
    int Variation_2();
    // @cmember DB_E_NULLACCESSORNOTSUPPORTED (80040E49)
    int Variation_3();
    // }} TCW_TESTVARS_END

    int TCAccessorErrors::VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pIAccessor, IID_IAccessor);
    }
};
// {{ TCW_TESTCASE(TCAccessorErrors)
#define THE_CLASS TCAccessorErrors
BEG_TEST_CASE(TCAccessorErrors, CCommandObject, L"IAccessor Error Messages")
TEST_VARIATION(1, 		L"DB_E_BADACCESSORHANDLE (80040E00)")
TEST_VARIATION(2, 		L"DB_E_BADACCESSORFLAGS (80040E46)")
TEST_VARIATION(3, 		L"DB_E_NULLACCESSORNOTSUPPORTED (80040E49)")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCCommandErrors)
//*-----------------------------------------------------------------------
// @class ICommand Error Messages
//
class TCCommandErrors : public CCommandObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCCommandErrors,CCommandObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_NOCOMMAND (80040E0C)
    int Variation_1();
    // @cmember DB_E_BADACCESSORHANDLE (80040E00)
    int Variation_2();
    // }} TCW_TESTVARS_END

    ICommandText *m_pICommandText;

    int VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pICommand, IID_ICommand);
    }
};
// {{ TCW_TESTCASE(TCCommandErrors)
#define THE_CLASS TCCommandErrors
BEG_TEST_CASE(TCCommandErrors, CCommandObject, L"ICommand Error Messages")
TEST_VARIATION(1, 		L"DB_E_NOCOMMAND (80040E0C)")
TEST_VARIATION(2, 		L"DB_E_BADACCESSORHANDLE (80040E00)")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCCommandPrepareErrors)
//*-----------------------------------------------------------------------
// @class ICommandPrepare Error Messages
//
class TCCommandPrepareErrors : public CCommandObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCCommandPrepareErrors,CCommandObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_NOCOMMAND (80040E0C)
    int Variation_1();
    // @cmember DB_E_OBJECTOPEN (80040E05)
    int Variation_2();
    // }} TCW_TESTVARS_END

    ICommandText *m_pICommandText;
    ICommandPrepare *m_pICommandPrepare;

    int VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pICommandPrepare, IID_ICommandPrepare);
    }
} ;
// {{ TCW_TESTCASE(TCCommandPrepareErrors)
#define THE_CLASS TCCommandPrepareErrors
BEG_TEST_CASE(TCCommandPrepareErrors, CCommandObject, L"ICommandPrepare Error Messages")
TEST_VARIATION(1, 		L"DB_E_NOCOMMAND (80040E0C)")
TEST_VARIATION(2, 		L"DB_E_OBJECTOPEN (80040E05)")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCCommandTextErrors)
//*-----------------------------------------------------------------------
// @class ICommandText Error Messages
//
class TCCommandTextErrors : public CCommandObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCCommandTextErrors,CCommandObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_DIALECTNOTSUPPORTED (80040E16)
    int Variation_1();
    // @cmember DB_S_DIALECTIGNORED (00040ECD)
    int Variation_2();
    // }} TCW_TESTVARS_END

    ICommandText *m_pICommandText;

    int VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pICommandText, IID_ICommandText);
    }
};

// {{ TCW_TESTCASE(TCCommandTextErrors)
#define THE_CLASS TCCommandTextErrors
BEG_TEST_CASE(TCCommandTextErrors, CCommandObject, L"ICommandText Error Messages")
TEST_VARIATION(1, 		L"DB_E_DIALECTNOTSUPPORTED (80040E16)")
TEST_VARIATION(2, 		L"DB_S_DIALECTIGNORED (00040ECD)")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCCommandWithParametersErrors)
//*-----------------------------------------------------------------------
// @class ICommandWithParameters Error Messages
//
class TCCommandWithParametersErrors : public CCommandObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCCommandWithParametersErrors,CCommandObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_NOTPREPARED (80040E4A)
    int Variation_1();
    // }} TCW_TESTVARS_END

    ICommandText *m_pICommandText;
    ICommandWithParameters *m_pICommandWithParameters;

    int VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pICommandWithParameters, IID_ICommandWithParameters);
    }
};
// {{ TCW_TESTCASE(TCCommandWithParametersErrors)
#define THE_CLASS TCCommandWithParametersErrors
BEG_TEST_CASE(TCCommandWithParametersErrors, CCommandObject, L"ICommandWithParameters Error Messages")
TEST_VARIATION(1, 		L"DB_E_NOTPREPARED (80040E4A)")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCOpenRowsetErrors)
//*-----------------------------------------------------------------------
// @class IOpenRowset Error Messages
//
class TCOpenRowsetErrors : public CSessionObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCOpenRowsetErrors,CSessionObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_NOTABLE (80040E37)
    int Variation_1();
    // @cmember DB_E_NOINDEX (80040E35)
    int Variation_2();
    // }} TCW_TESTVARS_END

    int VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pIOpenRowset, IID_IOpenRowset);
    }
};
// {{ TCW_TESTCASE(TCOpenRowsetErrors)
#define THE_CLASS TCOpenRowsetErrors
BEG_TEST_CASE(TCOpenRowsetErrors, CSessionObject, L"IOpenRowset Error Messages")
TEST_VARIATION(1, 		L"DB_E_NOTABLE (80040E37)")
TEST_VARIATION(2, 		L"DB_E_NOINDEX (80040E35)")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCRowsetErrors)
//*-----------------------------------------------------------------------
// @class IRowset Error Messages
//
class TCRowsetErrors : public CCommandObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCRowsetErrors,CCommandObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_CANTFETCHBACKWARDS (80040E24)
    int Variation_1();
    // @cmember DB_E_CANTSCROLLBACKWARDS (80040E29)
    int Variation_2();
    // @cmember DB_E_ROWSNOTRELEASED (80040E25)
    int Variation_3();
    // @cmember DB_E_BADACCESSORTYPE (80040E4B)
    int Variation_4();
    // @cmember DB_E_BADORDINAL (80040E55)
    int Variation_5();
    // @cmember DB_S_ROWLIMITEXCEEDED (00040EC0)
    int Variation_6();
    // }} TCW_TESTVARS_END

    IRowset *m_pIRowset;
    HACCESSOR m_hRowAccessor;
    HACCESSOR m_hParamAccessor;
    HACCESSOR m_hBadOrdinalAccessor;

    int VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pIRowset, IID_IRowset);
    }
};
// {{ TCW_TESTCASE(TCRowsetErrors)
#define THE_CLASS TCRowsetErrors
BEG_TEST_CASE(TCRowsetErrors, CCommandObject, L"IRowset Error Messages")
TEST_VARIATION(1, 		L"DB_E_CANTFETCHBACKWARDS (80040E24)")
TEST_VARIATION(2, 		L"DB_E_CANTSCROLLBACKWARDS (80040E29)")
TEST_VARIATION(3, 		L"DB_E_ROWSNOTRELEASED (80040E25)")
TEST_VARIATION(4, 		L"DB_E_BADACCESSORTYPE (80040E4B)")
TEST_VARIATION(5, 		L"DB_E_BADORDINAL (80040E55)")
TEST_VARIATION(6, 		L"DB_S_ROWLIMITEXCEEDED (00040EC0)")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCRowsetChangeErrors)
//*-----------------------------------------------------------------------
// @class IRowsetChange Error Messages
//
class TCRowsetChangeErrors : public CCommandObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCRowsetChangeErrors,CCommandObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_BADROWHANDLE (80040E04)
    int Variation_1();
    // @cmember DB_E_NOTSUPPORTED (80040E53)
    int Variation_2();
    // }} TCW_TESTVARS_END

    IRowset *m_pIRowset;
    IRowsetChange *m_pIRowsetChange;

    int VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pIRowsetChange, IID_IRowsetChange);
    }
};
// {{ TCW_TESTCASE(TCRowsetChangeErrors)
#define THE_CLASS TCRowsetChangeErrors
BEG_TEST_CASE(TCRowsetChangeErrors, CCommandObject, L"IRowsetChange Error Messages")
TEST_VARIATION(1, 		L"DB_E_BADROWHANDLE (80040E04)")
TEST_VARIATION(2, 		L"DB_E_NOTSUPPORTED (80040E53)")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCRowsetInfoErrors)
//*-----------------------------------------------------------------------
// @class IRowsetInfo Error Messages
//
class TCRowsetInfoErrors : public CCommandObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCRowsetInfoErrors,CCommandObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_NOTAREFERENCECOLUMN (80040E0A)
    int Variation_1();
    // @cmember DB_E_BADORDINAL (80040E55)
    int Variation_2();
    // }} TCW_TESTVARS_END

    IRowsetInfo *m_pIRowsetInfo;
    int VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pIRowsetInfo, IID_IRowsetInfo);
    }
} ;
// {{ TCW_TESTCASE(TCRowsetInfoErrors)
#define THE_CLASS TCRowsetInfoErrors
BEG_TEST_CASE(TCRowsetInfoErrors, CCommandObject, L"IRowsetInfo Error Messages")
TEST_VARIATION(1, 		L"DB_E_NOTAREFERENCECOLUMN (80040E0A)")
TEST_VARIATION(2, 		L"DB_E_BADORDINAL (80040E55)")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCRowsetLocateErrors)
//*-----------------------------------------------------------------------
// @class IRowsetLocate Error Messages
//
class TCRowsetLocateErrors : public CCommandObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCRowsetLocateErrors,CCommandObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_BADBOOKMARK (80040E0E)
    int Variation_1();
    // }} TCW_TESTVARS_END

    IRowsetLocate *m_pIRowsetLocate;
    int VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pIRowsetLocate, IID_IRowsetLocate);
    }
} ;
// {{ TCW_TESTCASE(TCRowsetLocateErrors)
#define THE_CLASS TCRowsetLocateErrors
BEG_TEST_CASE(TCRowsetLocateErrors, CCommandObject, L"IRowsetLocate Error Messages")
TEST_VARIATION(1, 		L"DB_E_BADBOOKMARK (80040E0E)")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(TCRowsetScrollErrors)
//*-----------------------------------------------------------------------
// @class IRowsetScroll Error Messages
//
class TCRowsetScrollErrors : public CCommandObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCRowsetScrollErrors,CCommandObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_BADRATIO (80040E12)
    int Variation_1();
    // }} TCW_TESTVARS_END

    IRowsetScroll *m_pIRowsetScroll;
    int VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pIRowsetScroll, IID_IRowsetScroll);
    }
} ;
// {{ TCW_TESTCASE(TCRowsetScrollErrors)
#define THE_CLASS TCRowsetScrollErrors
BEG_TEST_CASE(TCRowsetScrollErrors, CCommandObject, L"IRowsetScroll Error Messages")
TEST_VARIATION(1, 		L"DB_E_BADRATIO (80040E12)")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCRowsetUpdateErrors)
//*-----------------------------------------------------------------------
// @class IRowsetUpdate Error Messages
//
class TCRowsetUpdateErrors : public CCommandObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCRowsetUpdateErrors,CCommandObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_CANCELED
    int Variation_1();
    // }} TCW_TESTVARS_END

    IRowset *m_pIRowset;
    IRowsetUpdate *m_pIRowsetUpdate;
    int VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pIRowsetUpdate, IID_IRowsetUpdate);
    }
} ;
// {{ TCW_TESTCASE(TCRowsetUpdateErrors)
#define THE_CLASS TCRowsetUpdateErrors
BEG_TEST_CASE(TCRowsetUpdateErrors, CCommandObject, L"IRowsetUpdate Error Messages")
TEST_VARIATION(1, 		L"DB_E_CANCELED")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCTransactionLocalErrors)
//*-----------------------------------------------------------------------
// @class ITransactionLocal Error Messages
//
class TCTransactionLocalErrors : public CSessionObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCTransactionLocalErrors,CSessionObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember XACT_E_NOTRANSACTION (8004D00E)
    int Variation_1();
    // @cmember XACT_E_XTIONEXISTS (8004D013)
    int Variation_2();
    // @cmember XACT_E_ISOLATIONLEVEL (8004D008)
    int Variation_3();
    // }} TCW_TESTVARS_END

    ITransactionLocal *m_pITransactionLocal;
    int VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pITransactionLocal, IID_ITransactionLocal);
    }
};
// {{ TCW_TESTCASE(TCTransactionLocalErrors)
#define THE_CLASS TCTransactionLocalErrors
BEG_TEST_CASE(TCTransactionLocalErrors, CSessionObject, L"ITransactionLocal Error Messages")
TEST_VARIATION(1, 		L"XACT_E_NOTRANSACTION (8004D00E)")
TEST_VARIATION(2, 		L"XACT_E_XTIONEXISTS (8004D013)")
TEST_VARIATION(3,		L"XACT_E_ISOLATIONLEVEL(8004D008)")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCColumnsInfo)
//*-----------------------------------------------------------------------
// @class IColumnsInfo Error Messages
//
class TCColumnsInfo : public CCommandObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCColumnsInfo,CCommandObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_NOCOMMAND (80040E0C)
    int Variation_1();
    // }} TCW_TESTVARS_END

    IColumnsInfo *m_pIColumnsInfo;
    int VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pIColumnsInfo, IID_IColumnsInfo);
    }
} ;
// {{ TCW_TESTCASE(TCColumnsInfo)
#define THE_CLASS TCColumnsInfo
BEG_TEST_CASE(TCColumnsInfo, CCommandObject, L"IColumnsInfo Error Messages")
TEST_VARIATION(1, 		L"DB_E_NOCOMMAND (80040E0C)")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCColumnsRowsetErrors)
//*-----------------------------------------------------------------------
// @class IColumnsRowset Error Messages
//
class TCColumnsRowsetErrors : public CCommandObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCColumnsRowsetErrors,CCommandObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_BADCOLUMNID (80040E11)
    int Variation_1();
    // }} TCW_TESTVARS_END

    IColumnsRowset *m_pIColumnsRowset;
    int VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pIColumnsRowset, IID_IColumnsRowset);
    }
} ;
// {{ TCW_TESTCASE(TCColumnsRowsetErrors)
#define THE_CLASS TCColumnsRowsetErrors
BEG_TEST_CASE(TCColumnsRowsetErrors, CCommandObject, L"IColumnsRowset Error Messages")
TEST_VARIATION(1, 		L"DB_E_BADCOLUMNID (80040E11)")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCCommandPropertiesErrors)
//*-----------------------------------------------------------------------
// @class ICommandProperties Error Messages
//
class TCCommandPropertiesErrors : public CCommandObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCCommandPropertiesErrors,CCommandObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_OBJECTOPEN
    int Variation_1();
    // }} TCW_TESTVARS_END

    ICommandProperties *m_pICommandProperties;
    int VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pICommandProperties, IID_ICommandProperties);
    }
} ;
// {{ TCW_TESTCASE(TCCommandPropertiesErrors)
#define THE_CLASS TCCommandPropertiesErrors
BEG_TEST_CASE(TCCommandPropertiesErrors, CCommandObject, L"ICommandProperties Error Messages")
TEST_VARIATION(1, 		L"DB_E_OBJECTOPEN")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCConvertTypeErrors)
//*-----------------------------------------------------------------------
// @class IConvertType Error Messages
//
class TCConvertTypeErrors : public CCommandObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCConvertTypeErrors,CCommandObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_BADCONVERTFLAG
    int Variation_1();
    // @cmember DB_E_BADTYPE
    int Variation_2();
    // }} TCW_TESTVARS_END

    IConvertType *m_pIConvertType;
    int VerifyMessage(HRESULT hrExp, HRESULT hrAct)
    {
        return ::VerifyMessage(hrExp, hrAct, (IUnknown *)m_pIConvertType, IID_IConvertType);
    }
};
// {{ TCW_TESTCASE(TCConvertTypeErrors)
#define THE_CLASS TCConvertTypeErrors
BEG_TEST_CASE(TCConvertTypeErrors, CCommandObject, L"IConvertType Error Messages")
TEST_VARIATION(1, 		L"DB_E_BADCONVERTFLAG")
TEST_VARIATION(2, 		L"DB_E_BADTYPE")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCDBCreateCommandErrors)
//*-----------------------------------------------------------------------
// @class IDBCreateCommand Error Messages
//
class TCDBCreateCommandErrors : public CTestCases
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCDBCreateCommandErrors,CTestCases);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_NOAGGREGATION
    int Variation_1();
    // }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCDBCreateCommandErrors)
#define THE_CLASS TCDBCreateCommandErrors
BEG_TEST_CASE(TCDBCreateCommandErrors, CTestCases, L"IDBCreateCommand Error Messages")
TEST_VARIATION(1, 		L"DB_E_NOAGGREGATION")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCDBCreateSessionErrors)
//*-----------------------------------------------------------------------
// @class IDBCreateSession Error Messages
//
class TCDBCreateSessionErrors : public CTestCases
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCDBCreateSessionErrors,CTestCases);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_NOAGGREGATION
    int Variation_1();
    // }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCDBCreateSessionErrors)
#define THE_CLASS TCDBCreateSessionErrors
BEG_TEST_CASE(TCDBCreateSessionErrors, CTestCases, L"IDBCreateSession Error Messages")
TEST_VARIATION(1, 		L"DB_E_NOAGGREGATION")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCDBInitializeErrors)
//*-----------------------------------------------------------------------
// @class IDBInitialize Error Messages
//
class TCDBInitializeErrors : public CTestCases
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCDBInitializeErrors,CTestCases);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_ALREADYINITIALIZED
    int Variation_1();
    // }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCDBInitializeErrors)
#define THE_CLASS TCDBInitializeErrors
BEG_TEST_CASE(TCDBInitializeErrors, CTestCases, L"IDBInitialize Error Messages")
TEST_VARIATION(1, 		L"DB_E_ALREADYINITIALIZED")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCRowsetIdentityErrors)
//*-----------------------------------------------------------------------
// @class IRowsetIdentity Error Messages
//
class TCRowsetIdentityErrors : public CCommandObject
{
private:
    // @cmember Static array of variations
    DECLARE_TEST_CASE_DATA();

public:
    // {{ TCW_DECLARE_FUNCS
    // @cmember Execution Routine
    DECLARE_TEST_CASE_FUNCS(TCRowsetIdentityErrors,CCommandObject);
    // }} TCW_DECLARE_FUNCS_END

    // @cmember Initialization Routine
    virtual BOOL Init();
    // @cmember Termination Routine
    virtual BOOL Terminate();

    // {{ TCW_TESTVARS()
    // @cmember DB_E_BADROWHANDLE
    int Variation_1();
    // }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCRowsetIdentityErrors)
#define THE_CLASS TCRowsetIdentityErrors
BEG_TEST_CASE(TCRowsetIdentityErrors, CCommandObject, L"IRowsetIdentity Error Messages")
TEST_VARIATION(1, 		L"DB_E_BADROWHANDLE")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Test Case Section
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// }} END_DECLARE_TEST_CASES()

// {{ TCW_TESTMODULE(ThisModule)
TEST_MODULE(22, ThisModule, gwszModuleDescrip)
TEST_CASE(1, TCErrorMessages)
TEST_CASE(2, TCAccessorErrors)
TEST_CASE(3, TCCommandErrors)
TEST_CASE(4, TCCommandPrepareErrors)
TEST_CASE(5, TCCommandTextErrors)
TEST_CASE(6, TCCommandWithParametersErrors)
TEST_CASE(7, TCOpenRowsetErrors)
TEST_CASE(8, TCRowsetErrors)
TEST_CASE(9, TCRowsetChangeErrors)
TEST_CASE(10, TCRowsetInfoErrors)
TEST_CASE(11, TCRowsetLocateErrors)
TEST_CASE(12, TCRowsetScrollErrors)
TEST_CASE(13, TCRowsetUpdateErrors)
TEST_CASE(14, TCTransactionLocalErrors)
TEST_CASE(15, TCColumnsInfo)
TEST_CASE(16, TCColumnsRowsetErrors)
TEST_CASE(17, TCCommandPropertiesErrors)
TEST_CASE(18, TCConvertTypeErrors)
TEST_CASE(19, TCDBCreateCommandErrors)
TEST_CASE(20, TCDBCreateSessionErrors)
TEST_CASE(21, TCDBInitializeErrors)
TEST_CASE(22, TCRowsetIdentityErrors)
END_TEST_MODULE()
// }} TCW_TESTMODULE_END


// {{ TCW_TC_PROTOTYPE(TCErrorMessages)
//*-----------------------------------------------------------------------
//| Test Case:		TCErrorMessages -
//| Created:  	00/03/21
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCErrorMessages::Init()
{
    // {{ TCW_INIT_BASECLASS_CHECK
    if(CTestCases::Init())
        // }}
    {
        // TO DO:  Add your own code here
        return TRUE;
    }
    return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc Verify Messages
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCErrorMessages::Variation_1()
{
    int nResult = TEST_FAIL;
    int nFailCount = 0;
    IClassFactory *pIClassFactory = NULL;
    IErrorInfo *pIErrorInfo = NULL;
    IErrorRecords *pIErrorRecords = NULL;
    ERRORINFO ErrorInfo;
    BSTR bstrDescription = NULL;
    MESSAGE_ENTRY *pMsg;
    LPWSTR pwszHr;
    WCHAR wszBuff[64];

    // Create ClassFactory of OLE DB Error Object.
    TESTC_(CoGetClassObject(CLSID_EXTENDEDERRORINFO, CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory, (void **)&pIClassFactory), S_OK);

    // Walk through all error messages.
    for(pMsg=g_rgMsgs; pMsg->pwszDescription; pMsg++)
    {
        // print out HRESULT
        pwszHr = GetErrorName(pMsg->hr);
        if (pwszHr)
        {
            odtLog << pwszHr << "\n";
        }
        else
        {
            swprintf(wszBuff, L"%08X", pMsg->hr);
            odtLog << wszBuff << "\n";
        }

        // Create Instance of OLE DB Error Object.
        TESTC_(pIClassFactory->CreateInstance(NULL, IID_IErrorInfo, (void **)&pIErrorInfo), S_OK);
        TESTC_(pIErrorInfo->QueryInterface(IID_IErrorRecords, (void **)&pIErrorRecords), S_OK);

        // Push an HRESULT to a ErrorRecord.
        memset(&ErrorInfo, 0, sizeof(ErrorInfo));
        ErrorInfo.hrError = pMsg->hr;
        TESTC_(pIErrorRecords->AddErrorRecord(&ErrorInfo, IDENTIFIER_SDK_ERROR, NULL, NULL, 0),  S_OK);

        // Get Description of HRESULT just pushed above and compare.
        if (pIErrorInfo->GetDescription(&bstrDescription) == S_OK)
        {
            if (bstrDescription)
            {
                if (wcscmp(bstrDescription, pMsg->pwszDescription) == 0)
                {
                    odtLog << bstrDescription << "\n";
                    odtLog << "PASSED\n\n";
                }
                else
                {
                    odtLog << "Expect: " << pMsg->pwszDescription << "\n";
                    odtLog << "Actual: " << bstrDescription << "\n";
                    odtLog << "FAILED\n\n";
                    nFailCount++;
                }
                SysFreeString(bstrDescription);
            }
            else
            {
                odtLog << "Expect: " << pMsg->pwszDescription << "\n";
                odtLog << "Actual: (no description)\n";
                odtLog << "FAILED\n\n";
                nFailCount++;
            }
            bstrDescription = NULL;
        }
        else
        {
            nFailCount++;
            odtLog << "Expect: " << pMsg->pwszDescription << "\n";
            odtLog << "Actual: (no description)\n";
            odtLog << "FAILED\n\n";
        }
        SAFE_RELEASE(pIErrorRecords);
        SAFE_RELEASE(pIErrorInfo);
    }

    nResult = nFailCount == 0 ? TEST_PASS : TEST_FAIL;

CLEANUP:
    SAFE_RELEASE(pIErrorRecords);
    SAFE_RELEASE(pIErrorInfo);
    SAFE_RELEASE(pIClassFactory);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCErrorMessages::Terminate()
{
    // TO DO:  Add your own code here

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CTestCases::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCAccessorErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCAccessorErrors - IAccessor Error Messages
//| Created:  	9/27/1999
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCAccessorErrors::Init()
{
    // {{ TCW_INIT_BASECLASS_CHECK
    if(CCommandObject::Init())
        // }}
    {
        // TO DO:  Add your own code here
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        CreateCommandObject();
        return VerifyInterface(m_pICommand, IID_IAccessor, COMMAND_INTERFACE, (IUnknown**)&m_pIAccessor);
    }
    return FALSE;
}

// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_BADACCESSORHANDLE (80040E00)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAccessorErrors::Variation_1()
{
    ULONG ulTemp;
    return VerifyMessage(DB_E_BADACCESSORHANDLE, m_pIAccessor->AddRefAccessor(666, &ulTemp));
}
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc DB_E_BADACCESSORFLAGS (80040E46)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAccessorErrors::Variation_2()
{
    DBBINDING I4Binding = { 1,0,0,0,NULL,NULL,NULL,DBPART_VALUE,0,0,0,0,DBTYPE_I4,0,0 };
    HACCESSOR hAccessor;
    return VerifyMessage(DB_E_BADACCESSORFLAGS, m_pIAccessor->CreateAccessor(666, 1, &I4Binding, 0, &hAccessor, NULL));
}
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc DB_E_NULLACCESSORNOTSUPPORTED (80040E49)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAccessorErrors::Variation_3()
{
    DBBINDING ZeroBinding;
    HACCESSOR hAccessor;
    memset(&ZeroBinding, 0, sizeof(ZeroBinding));
    return VerifyMessage(DB_E_NULLACCESSORNOTSUPPORTED, m_pIAccessor->CreateAccessor(DBACCESSOR_ROWDATA, 0, NULL, 0, &hAccessor, NULL));
}
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCAccessorErrors::Terminate()
{
    // TO DO:  Add your own code here
    SAFE_RELEASE(m_pIAccessor);
    ReleaseCommandObject();
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CCommandObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END






// {{ TCW_TC_PROTOTYPE(TCCommandErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCCommandErrors - ICommand Error Messages
//| Created:  	00/03/08
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCCommandErrors::Init()
{
    BOOL bResult = FALSE;
    m_pICommandText = NULL;
    m_pTable = NULL;

    // {{ TCW_INIT_BASECLASS_CHECK
    if(CCommandObject::Init())
        // }}
    {
        // TO DO:  Add your own code here
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        SetTable((CTable *)m_pThisTestModule->m_pVoid, DELETETABLE_NO);
        TESTC(CreateCommandObject() == S_OK);
        TESTC(VerifyInterface(m_pICommand, IID_ICommandText, COMMAND_INTERFACE, (IUnknown**)&m_pICommandText));
        bResult = TRUE;
    }
CLEANUP:
    return bResult;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_NOCOMMAND (80040E0C)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCommandErrors::Variation_1()
{
    DBROWCOUNT lTemp = 0;
    return VerifyMessage(DB_E_NOCOMMAND, m_pICommand->Execute(NULL, IID_NULL, NULL, &lTemp, NULL));
}
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc DB_E_BADACCESSORHANDLE (80040E00)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCommandErrors::Variation_2()
{
    // TO DO:  Add your own code here
    DBPARAMS dbparams;
    DBROWCOUNT lTemp = 0;
    WCHAR*		pSQLSet = NULL;
    CCol intCol;
    int nResult = 0;
    HRESULT hr = E_FAIL;

    dbparams.pData = &lTemp;
    dbparams.cParamSets = 1;
    dbparams.hAccessor = 666;

    if(FAILED(m_pTable->GetColInfo(intCol, DBTYPE_I4)))
        return TEST_SKIPPED;

    //swprintf(wszCommandText, L"INSERT INTO %s (%s) VALUES (?)", m_pTable->GetTableName(), intCol.GetColName());
    TEST2C_(hr=m_pTable->CreateSQLStmt(INSERT_ALLWITHPARAMS,
                                       NULL, &pSQLSet, NULL, NULL),S_OK,DB_E_NOTSUPPORTED);
    if(hr == DB_E_NOTSUPPORTED)
    {
        nResult = TEST_SKIPPED;
        goto CLEANUP;
    }

    m_pICommandText->SetCommandText(DBGUID_DBSQL, pSQLSet);
    nResult = VerifyMessage(DB_E_BADACCESSORHANDLE, m_pICommand->Execute(NULL, IID_NULL, &dbparams, &lTemp, NULL));

CLEANUP:
    PROVIDER_FREE(pSQLSet);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCCommandErrors::Terminate()
{
    SAFE_RELEASE(m_pICommandText);
    ReleaseCommandObject();
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CCommandObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCCommandPrepareErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCCommandPrepareErrors - ICommandPrepare Error Messages
//| Created:  	00/03/08
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCCommandPrepareErrors::Init()
{
    BOOL bResult = FALSE;
    m_pICommandText = NULL;
    m_pICommandPrepare = NULL;

    // {{ TCW_INIT_BASECLASS_CHECK
    if(CCommandObject::Init())
        // }}
    {
        // TO DO:  Add your own code here
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        SetTable((CTable *)m_pThisTestModule->m_pVoid, DELETETABLE_NO);
        TESTC(CreateCommandObject() == S_OK);
        TESTC(VerifyInterface(m_pICommand, IID_ICommandText, COMMAND_INTERFACE, (IUnknown**)&m_pICommandText));
        TESTC(VerifyInterface(m_pICommand, IID_ICommandPrepare, COMMAND_INTERFACE, (IUnknown**)&m_pICommandPrepare));
        bResult = TRUE;
    }
CLEANUP:
    return bResult;
}




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_NOCOMMAND (80040E0C)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCommandPrepareErrors::Variation_1()
{
    m_pICommandText->SetCommandText(DBGUID_DBSQL, NULL);
    return VerifyMessage(DB_E_NOCOMMAND, m_pICommandPrepare->Prepare(0));
}
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc DB_E_OBJECTOPEN (80040E05)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCommandPrepareErrors::Variation_2()
{
    int nResult = TEST_FAIL;
    WCHAR*		pSQLSet = NULL;
    IUnknown *pIUnknown = NULL;

    TESTC(SUCCEEDED(m_pTable->CreateSQLStmt(SELECT_ALLFROMTBL,
                                            NULL, &pSQLSet, NULL, NULL)));
    TESTC_(m_pICommandText->SetCommandText(DBGUID_DBSQL, pSQLSet), S_OK);

    TESTC_(m_pICommand->Execute(NULL, IID_IUnknown, NULL, NULL, &pIUnknown), S_OK);

    nResult = VerifyMessage(DB_E_OBJECTOPEN, m_pICommandPrepare->Prepare(0));

CLEANUP:
    SAFE_RELEASE(pIUnknown);
    PROVIDER_FREE(pSQLSet);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCCommandPrepareErrors::Terminate()
{
    // TO DO:  Add your own code here
    SAFE_RELEASE(m_pICommandText);
    SAFE_RELEASE(m_pICommandPrepare);
    ReleaseCommandObject();
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CCommandObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCCommandTextErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCCommandTextErrors - ICommandText Error Messages
//| Created:  	00/03/08
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCCommandTextErrors::Init()
{
    BOOL bResult = FALSE;
    m_pICommandText = NULL;

    // {{ TCW_INIT_BASECLASS_CHECK
    if(CCommandObject::Init())
        // }}
    {
        // TO DO:  Add your own code here
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        SetTable((CTable *)m_pThisTestModule->m_pVoid, DELETETABLE_NO);
        TESTC(CreateCommandObject() == S_OK);
        TESTC(VerifyInterface(m_pICommand, IID_ICommandText, COMMAND_INTERFACE, (IUnknown**)&m_pICommandText));
        bResult = TRUE;
    }
CLEANUP:
    return bResult;
}




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_DIALECTNOTSUPPORTED (80040E16)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCommandTextErrors::Variation_1()
{
    return VerifyMessage(DB_E_DIALECTNOTSUPPORTED, m_pICommandText->SetCommandText(IID_IUnknown, L"Dummy Command"));
}
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc DB_S_DIALECTIGNORED (00040ECD)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCommandTextErrors::Variation_2()
{
    int nResult = TEST_FAIL;
    WCHAR*		pSQLSet = NULL;
    GUID guidDialect;
    LPWSTR pwszCommandText = NULL;

    TESTC(SUCCEEDED(m_pTable->CreateSQLStmt(SELECT_ALLFROMTBL,
                                            NULL, &pSQLSet, NULL, NULL)))
    TESTC_(m_pICommandText->SetCommandText(DBGUID_DBSQL, pSQLSet), S_OK);
    guidDialect = IID_IUnknown;
    nResult = VerifyMessage(DB_S_DIALECTIGNORED, m_pICommandText->GetCommandText(&guidDialect, &pwszCommandText));

CLEANUP:
    SAFE_FREE(pwszCommandText);
    PROVIDER_FREE(pSQLSet);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCCommandTextErrors::Terminate()
{
    // TO DO:  Add your own code here
    SAFE_RELEASE(m_pICommandText);
    ReleaseCommandObject();
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CCommandObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCCommandWithParametersErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCCommandWithParametersErrors - ICommandWithParameters Error Messages
//| Created:  	00/03/08
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCCommandWithParametersErrors::Init()
{
    BOOL bResult = FALSE;
    m_pICommandText = NULL;
    m_pICommandWithParameters = NULL;
    HRESULT hr = E_FAIL;

    // {{ TCW_INIT_BASECLASS_CHECK
    if(CCommandObject::Init())
        // }}
    {
        // TO DO:  Add your own code here
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        TESTC(CreateCommandObject() == S_OK);
        TESTC(VerifyInterface(m_pICommand, IID_ICommandText, COMMAND_INTERFACE, (IUnknown**)&m_pICommandText));
        //TESTC(VerifyInterface(m_pICommand, IID_ICommandWithParameters, COMMAND_INTERFACE, (IUnknown**)&m_pICommandWithParameters));
        if(!VerifyInterface(m_pICommand, IID_ICommandWithParameters, COMMAND_INTERFACE, (IUnknown**)&m_pICommandWithParameters))
        {
            odtLog << "Provider does not support ICommandWithParameters Interface\n";
            bResult = TEST_SKIPPED;
            goto CLEANUP;
        }

        bResult = TRUE;

    }
CLEANUP:
    return bResult;
}




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_NOTPREPARED (80040E4A)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCommandWithParametersErrors::Variation_1()
{
    int nResult = TEST_FAIL;
    DB_UPARAMS cParams = 0;
    DBPARAMINFO *rgParamInfo = NULL;
    OLECHAR *pNamesBuffer = NULL;
    HRESULT	hr;

    m_pICommandText->SetCommandText(DBGUID_DBSQL, L"{ call foo(?) }");
    hr = m_pICommandWithParameters->GetParameterInfo(&cParams, &rgParamInfo, &pNamesBuffer);
    if( hr == DB_E_NOTPREPARED )
        nResult = VerifyMessage(DB_E_NOTPREPARED, hr);
    else if( hr == DB_E_PARAMUNAVAILABLE )
        nResult = VerifyMessage(DB_E_PARAMUNAVAILABLE, hr);
    else
        TESTC_(hr, DB_E_NOTPREPARED );

CLEANUP:

    SAFE_FREE(rgParamInfo);		// must always be NULL.
    SAFE_FREE(pNamesBuffer);	// must always be NULL.
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCCommandWithParametersErrors::Terminate()
{
    // TO DO:  Add your own code here
    SAFE_RELEASE(m_pICommandWithParameters);
    SAFE_RELEASE(m_pICommandText);
    ReleaseCommandObject();
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CCommandObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END




// {{ TCW_TC_PROTOTYPE(TCOpenRowsetErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCOpenRowsetErrors - IOpenRowset Error Messages
//| Created:  	00/03/09
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCOpenRowsetErrors::Init()
{
    // {{ TCW_INIT_BASECLASS_CHECK
    if(CSessionObject::Init())
        // }}
    {
        // TO DO:  Add your own code here
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        SetTable((CTable *)m_pThisTestModule->m_pVoid, DELETETABLE_NO);
        return TRUE;
    }
    return FALSE;
}




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_NOTABLE (80040E37)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCOpenRowsetErrors::Variation_1()
{
    // TO DO:  Add your own code here
    int nResult = TEST_FAIL;
    DBID tblid;
    IUnknown *pIUnknown = NULL;

    tblid.eKind = DBKIND_NAME;
    tblid.uName.pwszName = L"";
    nResult = VerifyMessage(DB_E_NOTABLE, m_pIOpenRowset->OpenRowset(NULL, &tblid, NULL, IID_IUnknown, 0, NULL, &pIUnknown));

    SAFE_RELEASE(pIUnknown);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc DB_E_NOINDEX (80040E35)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCOpenRowsetErrors::Variation_2()
{
    // TO DO:  Add your own code here
    int nResult = TEST_FAIL;
    DBID tblid;
    DBID idxid;
    IUnknown *pIUnknown = NULL;

    tblid.eKind = DBKIND_NAME;
//	tblid.uName.pwszName = ((CTable *)m_pThisTestModule->m_pVoid)->GetTableName();
    tblid.uName.pwszName = m_pTable->GetTableName();

    idxid.eKind = DBKIND_NAME;
    idxid.uName.pwszName = L"does not exist";

    nResult = VerifyMessage(DB_E_NOINDEX, m_pIOpenRowset->OpenRowset(NULL, &tblid, &idxid, IID_IUnknown, 0, NULL, &pIUnknown));

    SAFE_RELEASE(pIUnknown);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCOpenRowsetErrors::Terminate()
{
    // TO DO:  Add your own code here
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CSessionObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCRowsetErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCRowsetErrors - IRowset Error Messages
//| Created:  	00/03/09
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCRowsetErrors::Init()
{
    BOOL bResult = FALSE;
    DBBINDING binding;
    WCHAR*		pSQLStmt = NULL;
    m_pIRowset = NULL;
    m_hRowAccessor = NULL;
    m_hParamAccessor = NULL;
    m_hBadOrdinalAccessor = NULL;
    HRESULT hr = E_FAIL;

    // {{ TCW_INIT_BASECLASS_CHECK
    if(CCommandObject::Init())
        // }}
    {
        // TO DO:  Add your own code here
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        SetTable((CTable *)m_pThisTestModule->m_pVoid, DELETETABLE_NO);
        CreateCommandObject();

        // Creating Valid Row Accessor
        memset(&binding, 0, sizeof(binding));
        binding.iOrdinal = 1;
        binding.dwPart = DBPART_VALUE;
        binding.wType = DBTYPE_I4;
        TESTC(VerifyInterface(m_pICommand, IID_IAccessor, COMMAND_INTERFACE, (IUnknown**)&m_pIAccessor));
        TESTC_(m_pIAccessor->CreateAccessor(DBACCESSOR_ROWDATA, 1, &binding, 0, &m_hRowAccessor, NULL), S_OK);

        // Creating Valid Parameter Accessor
        binding.eParamIO = DBPARAMIO_INPUT;
        TEST2C_(hr = m_pIAccessor->CreateAccessor(DBACCESSOR_PARAMETERDATA, 1, &binding, 0, &m_hParamAccessor, NULL), S_OK,DB_E_BADACCESSORFLAGS);
        if (hr == DB_E_BADACCESSORFLAGS)
        {
            odtLog<<" Provider does not Support Parameters \n";
        }


        // Creating Invalid Row Accessor
        binding.eParamIO = DBPARAMIO_NOTPARAM;
        binding.iOrdinal = 666;
        TESTC_(m_pIAccessor->CreateAccessor(DBACCESSOR_ROWDATA, 1, &binding, 0, &m_hBadOrdinalAccessor, NULL), S_OK);

        // Creating Rowset Object
        //swprintf(wszCommandText, L"SELECT * FROM %s", m_pTable->GetTableName());
        TESTC(SUCCEEDED(m_pTable->CreateSQLStmt(SELECT_ALLFROMTBL,
                                                NULL, &pSQLStmt, NULL, NULL)));
        TESTC_(::ExecuteCommand(m_pICommand, pSQLStmt, IID_IRowset, (IUnknown **)&m_pIRowset), S_OK);

        bResult = TRUE;
    }
CLEANUP:
    PROVIDER_FREE(pSQLStmt);
    return bResult;
}




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_CANTFETCHBACKWARDS (80040E24)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCRowsetErrors::Variation_1()
{
    // TO DO:  Add your own code here
    int nResult = TEST_FAIL;
    HROW hRow = NULL;
    HROW *phRow = &hRow;
    DBCOUNTITEM ulTemp = 0;

    if(GetProperty(DBPROP_CANFETCHBACKWARDS, DBPROPSET_ROWSET,
                   (IUnknown*)m_pIRowset,VARIANT_TRUE))
    {
        odtLog << L" Rowset supports backward fetching. \n";
        return TEST_SKIPPED;
    }


    nResult = VerifyMessage(DB_E_CANTFETCHBACKWARDS,m_pIRowset->GetNextRows(NULL, 1, -1, &ulTemp, &phRow));

//CLEANUP:
    if (hRow)
        m_pIRowset->ReleaseRows(1, &hRow, NULL, NULL, NULL);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END





// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc DB_E_CANTSCROLLBACKWARDS (80040E29)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCRowsetErrors::Variation_2()
{
    // TO DO:  Add your own code here
    int nResult = TEST_FAIL;
    HROW hRow = NULL;
    HROW *phRow = &hRow;
    DBCOUNTITEM ulTemp = 0;

    if(GetProperty(DBPROP_CANSCROLLBACKWARDS, DBPROPSET_ROWSET,
                   (IUnknown*)m_pIRowset,VARIANT_TRUE))
    {
        odtLog << L" Rowset supports backward scrolling. \n";
        return TEST_SKIPPED;
    }


    nResult = VerifyMessage(DB_E_CANTSCROLLBACKWARDS, m_pIRowset->GetNextRows(NULL, -1, 1, &ulTemp, &phRow));

//CLEANUP:
    if (hRow)
        m_pIRowset->ReleaseRows(1, &hRow, NULL, NULL, NULL);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc DB_E_ROWSNOTRELEASED (80040E25)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCRowsetErrors::Variation_3()
{
    // TO DO:  Add your own code here
    int nResult = TEST_FAIL;
    HROW hRow1 = NULL;
    HROW hRow2 = NULL;
    HROW *phRow;
    DBCOUNTITEM ulTemp = 0;

    if(GetProperty(DBPROP_CANHOLDROWS, DBPROPSET_ROWSET, m_pIRowset, VARIANT_TRUE))
        return TEST_SKIPPED;

    m_pIRowset->RestartPosition(NULL);

    // Fetch First Row
    phRow = &hRow1;
    TESTC_(m_pIRowset->GetNextRows(NULL, 0, 1, &ulTemp, &phRow), S_OK);

    // Fetch Second Row
    phRow = &hRow2;
    nResult = VerifyMessage(DB_E_ROWSNOTRELEASED, m_pIRowset->GetNextRows(NULL, 0, 1, &ulTemp, &phRow));

CLEANUP:
    if (hRow1)
        m_pIRowset->ReleaseRows(1, &hRow1, NULL, NULL, NULL);
    if (hRow2)
        m_pIRowset->ReleaseRows(1, &hRow2, NULL, NULL, NULL);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc DB_E_BADACCESSORTYPE (80040E4B)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCRowsetErrors::Variation_4()
{
    // TO DO:  Add your own code here
    int nResult = TEST_FAIL;
    HROW hRow = NULL;
    HROW *phRow;
    DBCOUNTITEM ulTemp = 0;
    LONG lTemp = 0;

    m_pIRowset->RestartPosition(NULL);

    phRow = &hRow;
    TESTC_(m_pIRowset->GetNextRows(NULL, 0, 1, &ulTemp, &phRow), S_OK);

    nResult = VerifyMessage(DB_E_BADACCESSORTYPE, m_pIRowset->GetData(hRow, m_hParamAccessor, &lTemp));

CLEANUP:
//	SAFE_RELEASE_ROW(m_pIRowset, hRow);
    if (hRow)
        m_pIRowset->ReleaseRows(1, &hRow, NULL, NULL, NULL);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc DB_E_BADORDINAL (80040E55)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCRowsetErrors::Variation_5()
{
    int nResult = TEST_FAIL;
    HROW hRow = NULL;
    HROW *phRow;
    DBCOUNTITEM ulTemp = 0;
    LONG lTemp = 0;

    m_pIRowset->RestartPosition(NULL);

    phRow = &hRow;
    TESTC_(m_pIRowset->GetNextRows(NULL, 0, 1, &ulTemp, &phRow), S_OK);

    nResult = VerifyMessage(DB_E_BADORDINAL, m_pIRowset->GetData(hRow, m_hBadOrdinalAccessor, &lTemp));

CLEANUP:
    if (hRow)
        m_pIRowset->ReleaseRows(1, &hRow, NULL, NULL, NULL);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END





// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc DB_S_ROWLIMITEXCEEDED (00040EC0)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCRowsetErrors::Variation_6()
{
    // TO DO:  Add your own code here
    return TRUE;
}
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCRowsetErrors::Terminate()
{
    // TO DO:  Add your own code here
    SAFE_RELEASE_ACCESSOR(m_pIAccessor, m_hRowAccessor);
    SAFE_RELEASE_ACCESSOR(m_pIAccessor, m_hParamAccessor);
    SAFE_RELEASE_ACCESSOR(m_pIAccessor, m_hBadOrdinalAccessor);
    SAFE_RELEASE(m_pIAccessor);
    SAFE_RELEASE(m_pIRowset);
    ReleaseCommandObject();
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CCommandObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCRowsetChangeErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCRowsetChangeErrors - IRowsetChange Error Messages
//| Created:  	00/03/09
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCRowsetChangeErrors::Init()
{
    BOOL		bResult = FALSE;
    WCHAR*		pSQLSet = NULL;
    CCol		intCol;
    LONG_PTR	dbPropValue = DBPROPVAL_UP_INSERT|DBPROPVAL_UP_CHANGE;

    m_pIRowset = NULL;
    m_pIRowsetChange = NULL;

    // {{ TCW_INIT_BASECLASS_CHECK
    if(CCommandObject::Init())
        // }}
    {
        // TO DO:  Add your own code here
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        SetTable((CTable *)m_pThisTestModule->m_pVoid, DELETETABLE_NO);
        CreateCommandObject();

        // Create RowsetObject with IRowsetChange
        SetRowsetProperty(m_pICommand, DBPROPSET_ROWSET, DBPROP_IRowsetChange, TRUE);
        SetRowsetProperty(m_pICommand, DBPROPSET_ROWSET, DBPROP_UPDATABILITY, dbPropValue);
        //m_pTable->GetFirstNumericCol(&intCol);
        //swprintf(wszCommandText, L"SELECT %s FROM %s", intCol.GetColName(), m_pTable->GetTableName());
        TESTC(SUCCEEDED(m_pTable->CreateSQLStmt(SELECT_ALLFROMTBL,
                                                NULL, &pSQLSet, NULL, NULL)));
        TESTC_(ExecuteCommand(m_pICommand, pSQLSet, IID_IRowset, (IUnknown **)&m_pIRowset), S_OK);
        if(!VerifyInterface(m_pIRowset, IID_IRowsetChange, ROWSET_INTERFACE, (IUnknown**)&m_pIRowsetChange))
        {
            bResult = TEST_SKIPPED;
            goto CLEANUP;
        }
        TESTC(VerifyInterface(m_pIRowset, IID_IAccessor, ROWSET_INTERFACE, (IUnknown**)&m_pIAccessor));
        bResult = TRUE;
    }
CLEANUP:
    PROVIDER_FREE(pSQLSet);
    return bResult;
}




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_BADROWHANDLE (80040E04)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCRowsetChangeErrors::Variation_1()
{
    int nResult = TEST_FAIL;
    DBBINDING binding = { 1,0,0,0,NULL,NULL,NULL,DBPART_VALUE,0,0,0,0,DBTYPE_I4,0,0 };
    HACCESSOR hAccessor = NULL;
    LONG lTemp;

    TESTC_(m_pIAccessor->CreateAccessor(DBACCESSOR_ROWDATA, 1, &binding, 0, &hAccessor, NULL), S_OK);

    nResult = VerifyMessage(DB_E_BADROWHANDLE, m_pIRowsetChange->SetData(666, hAccessor, (void *)&lTemp));

CLEANUP:
    SAFE_RELEASE_ACCESSOR(m_pIAccessor, hAccessor);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc DB_E_NOTSUPPORTED (80040E53)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCRowsetChangeErrors::Variation_2()
{
    // TO DO:  Add your own code here
    int nResult = TEST_FAIL;
    HROW hRow = NULL;
    HROW *phRow = &hRow;
    DBCOUNTITEM ulTemp;
    HRESULT hr = E_FAIL;

    m_pIRowset->RestartPosition(NULL);
    TESTC_(m_pIRowset->GetNextRows(NULL, 0, 1, &ulTemp, &phRow), S_OK);
    hr = m_pIRowsetChange->DeleteRows(NULL, 1, &hRow, NULL);
    if ( FAILED(hr))
    {
        nResult = VerifyMessage(DB_E_NOTSUPPORTED,hr);
    }
    else
    {
        nResult = TEST_SKIPPED;
    }

CLEANUP:
    if (hRow)
        m_pIRowset->ReleaseRows(1, &hRow, NULL, NULL, NULL);
//	SAFE_RELEASE_ACCESSOR(m_pIAccessor, hAccessor);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCRowsetChangeErrors::Terminate()
{
    // TO DO:  Add your own code here
    SAFE_RELEASE(m_pIAccessor);
    SAFE_RELEASE(m_pIRowsetChange);
    SAFE_RELEASE(m_pIRowset);
    ReleaseCommandObject();
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CCommandObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCRowsetInfoErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCRowsetInfoErrors - IRowsetInfo Error Messages
//| Created:  	00/03/13
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCRowsetInfoErrors::Init()
{
    BOOL bResult = FALSE;
//	ICommandText *pICommandText = NULL;
    WCHAR*		pSQLSet = NULL;
    m_pIRowsetInfo = NULL;

    // {{ TCW_INIT_BASECLASS_CHECK
    if(CCommandObject::Init())
        // }}
    {
        // TO DO:  Add your own code here
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        SetTable((CTable *)m_pThisTestModule->m_pVoid, DELETETABLE_NO);
        CreateCommandObject();

        // Create RowsetObject with IRowsetChange
        TESTC(SUCCEEDED(m_pTable->CreateSQLStmt(SELECT_ALLFROMTBL,
                                                NULL, &pSQLSet, NULL, NULL)));
//		TESTC(VerifyInterface(m_pICommand, IID_ICommandText, COMMAND_INTERFACE, (IUnknown**)&pICommandText));
//		TESTC_(pICommandText->SetCommandText(DBGUID_DBSQL, wszCommandText), S_OK);
//		TESTC_(m_pICommand->Execute(NULL, IID_IRowsetInfo, NULL, NULL, (IUnknown **)&m_pIRowsetInfo), S_OK);
        TESTC_(::ExecuteCommand(m_pICommand, pSQLSet, IID_IRowsetInfo, (IUnknown **)&m_pIRowsetInfo), S_OK);
        bResult = TRUE;
    }
CLEANUP:
//	SAFE_RELEASE(pICommandText);
    PROVIDER_FREE(pSQLSet);
    return bResult;
}




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_NOTAREFERENCECOLUMN (80040E0A)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCRowsetInfoErrors::Variation_1()
{
    int nResult = TEST_FAIL;
    IUnknown *pIUnknown = NULL;
    nResult = VerifyMessage(DB_E_NOTAREFERENCECOLUMN, m_pIRowsetInfo->GetReferencedRowset(1, IID_IUnknown, &pIUnknown));
    SAFE_RELEASE(pIUnknown);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc DB_E_BADORDINAL (80040E55)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCRowsetInfoErrors::Variation_2()
{
    int nResult = TEST_FAIL;
    IUnknown *pIUnknown = NULL;
    nResult = VerifyMessage(DB_E_BADORDINAL, m_pIRowsetInfo->GetReferencedRowset(666, IID_IUnknown, &pIUnknown));
    SAFE_RELEASE(pIUnknown);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCRowsetInfoErrors::Terminate()
{
    // TO DO:  Add your own code here
    SAFE_RELEASE(m_pIRowsetInfo);
    ReleaseCommandObject();
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CCommandObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCRowsetLocateErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCRowsetLocateErrors - IRowsetLocate Error Messages
//| Created:  	00/03/13
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCRowsetLocateErrors::Init()
{
    BOOL bResult = FALSE;
    HRESULT	hr;
    WCHAR*		pSQLSet = NULL;

    m_pIRowsetLocate = NULL;

    // {{ TCW_INIT_BASECLASS_CHECK
    if(CCommandObject::Init())
        // }}
    {
        // TO DO:  Add your own code here
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        SetTable((CTable *)m_pThisTestModule->m_pVoid, DELETETABLE_NO);
        CreateCommandObject();

        // Create RowsetObject with IRowsetLocate
        SetRowsetProperty(m_pICommand, DBPROPSET_ROWSET, DBPROP_IRowsetLocate, TRUE);
        TESTC(SUCCEEDED(m_pTable->CreateSQLStmt(SELECT_ALLFROMTBL,
                                                NULL, &pSQLSet, NULL, NULL)));

        hr = ::ExecuteCommand(m_pICommand, pSQLSet, IID_IRowsetLocate, (IUnknown **)&m_pIRowsetLocate);
        if( FAILED(hr) )
        {
            if(E_NOINTERFACE == hr)
                bResult = TEST_SKIPPED;

            goto CLEANUP;
        }

        bResult = TRUE;
    }
CLEANUP:
//	SAFE_RELEASE(pICommandText);
    return bResult;
}




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_BADBOOKMARK (80040E0E)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCRowsetLocateErrors::Variation_1()
{
    BYTE InvalidBmk = DBBMK_INVALID;
    DBCOMPARE fCompare;
    return VerifyMessage(DB_E_BADBOOKMARK, m_pIRowsetLocate->Compare(NULL, 1, &InvalidBmk, 1, &InvalidBmk, &fCompare));
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCRowsetLocateErrors::Terminate()
{
    // TO DO:  Add your own code here
    SAFE_RELEASE(m_pIRowsetLocate);
    ReleaseCommandObject();
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CCommandObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END



// {{ TCW_TC_PROTOTYPE(TCRowsetScrollErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCRowsetScrollErrors - IRowsetScroll Error Messages
//| Created:  	00/03/13
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCRowsetScrollErrors::Init()
{
    BOOL bResult = FALSE;
    HRESULT	hr;
    WCHAR*		pSQLSet = NULL;

    m_pIRowsetScroll = NULL;

    // {{ TCW_INIT_BASECLASS_CHECK
    if(CCommandObject::Init())
        // }}
    {
        // TO DO:  Add your own code here
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        SetTable((CTable *)m_pThisTestModule->m_pVoid, DELETETABLE_NO);
        CreateCommandObject();

        // Create RowsetObject with IRowsetLocate
        SetRowsetProperty(m_pICommand, DBPROPSET_ROWSET, DBPROP_IRowsetScroll, TRUE);
        TESTC(SUCCEEDED(m_pTable->CreateSQLStmt(SELECT_ALLFROMTBL,
                                                NULL, &pSQLSet, NULL, NULL)));
        hr = ::ExecuteCommand(m_pICommand, pSQLSet, IID_IRowsetScroll, (IUnknown **)&m_pIRowsetScroll);
        if( FAILED(hr) )
        {
            if(E_NOINTERFACE == hr)
                bResult = TEST_SKIPPED;

            goto CLEANUP;
        }

        bResult = TRUE;
    }
CLEANUP:
//	SAFE_RELEASE(pICommandText);
    PROVIDER_FREE(pSQLSet);
    return bResult;
}




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_BADRATIO (80040E12)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCRowsetScrollErrors::Variation_1()
{
    // TO DO:  Add your own code here
    int nResult = TEST_FAIL;
    DBCOUNTITEM ulTemp;
    HROW hRow = NULL;
    HROW *rghRows = &hRow;

    nResult = VerifyMessage(DB_E_BADRATIO, m_pIRowsetScroll->GetRowsAtRatio(NULL, NULL, 100, 1, 10, &ulTemp, &rghRows));
    TESTC(hRow == NULL);	// assert
CLEANUP:
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCRowsetScrollErrors::Terminate()
{
    // TO DO:  Add your own code here
    SAFE_RELEASE(m_pIRowsetScroll);
    ReleaseCommandObject();
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CCommandObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCRowsetUpdateErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCRowsetUpdateErrors - IRowsetUpdate Error Messages
//| Created:  	00/03/13
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCRowsetUpdateErrors::Init()
{
    BOOL bResult = FALSE;
    WCHAR*		pSQLSet = NULL;
    CCol intCol;
    LONG_PTR dbPropVal = DBPROPVAL_UP_INSERT|DBPROPVAL_UP_CHANGE|DBPROPVAL_UP_DELETE;

    m_pIRowset = NULL;
    m_pIRowsetUpdate = NULL;

    // {{ TCW_INIT_BASECLASS_CHECK
    if(CCommandObject::Init())
        // }}
    {
        // TO DO:  Add your own code here
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        SetTable((CTable *)m_pThisTestModule->m_pVoid, DELETETABLE_NO);
        CreateCommandObject();

        // Create RowsetObject with IRowsetChange
        SetRowsetProperty(m_pICommand, DBPROPSET_ROWSET, DBPROP_IConnectionPointContainer, TRUE);
//		SetRowsetProperty(m_pICommand, DBPROPSET_ROWSET, DBPROP_IRowsetChange, TRUE);
        SetRowsetProperty(m_pICommand, DBPROPSET_ROWSET, DBPROP_IRowsetUpdate, TRUE);
        SetRowsetProperty(m_pICommand, DBPROPSET_ROWSET, DBPROP_UPDATABILITY, dbPropVal);
        //m_pTable->GetFirstNumericCol(&intCol);
        TESTC(SUCCEEDED(m_pTable->CreateSQLStmt(SELECT_ALLFROMTBL,
                                                NULL, &pSQLSet, NULL, NULL)));
        TESTC_(ExecuteCommand(m_pICommand, pSQLSet, IID_IRowset, (IUnknown **)&m_pIRowset), S_OK);
        if(!VerifyInterface(m_pIRowset, IID_IRowsetUpdate, ROWSET_INTERFACE, (IUnknown**)&m_pIRowsetUpdate))
        {
            bResult = TEST_SKIPPED;
            goto CLEANUP;
        }
        TESTC(VerifyInterface(m_pIRowset, IID_IAccessor, ROWSET_INTERFACE, (IUnknown**)&m_pIAccessor));
        bResult = TRUE;
    }
CLEANUP:
    return bResult;
}

class TCRowsetUpdateNotify : public IRowsetNotify
{
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
    {
        if (riid == IID_IUnknown || riid == IID_IRowsetNotify)
        {
            *ppvObject = this;
            ((IUnknown *)(*ppvObject))->AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return 1;
    }
    ULONG STDMETHODCALLTYPE Release(void)
    {
        return 1;
    }

    //@cmember Notification Callback for Field Change
    STDMETHODIMP OnFieldChange(
        IRowset* pRowset,
        HROW hRow,
        DBCOUNTITEM cColumns,
        DBCOUNTITEM rgColumns[],
        DBREASON eReason,
        DBEVENTPHASE ePhase,
        BOOL fCantDeny)
    {
        return S_OK;
    }
    //@cmember Notification Callback for Row Change
    STDMETHODIMP OnRowChange(IRowset* pRowset,
                             DBCOUNTITEM cRows,
                             const HROW rghRows[],
                             DBREASON eReason,
                             DBEVENTPHASE ePhase,
                             BOOL fCantDeny)
    {
        if (fCantDeny)
            return S_OK;
        return S_FALSE;
    }
    //@cmember Notification Callback for Rowset Change
    STDMETHODIMP OnRowsetChange(IRowset*     pRowset,
                                DBREASON     eReason,
                                DBEVENTPHASE ePhase,
                                BOOL         fCantDeny)
    {
        if (fCantDeny)
            return S_OK;
        return S_FALSE;
    }
};

// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_CANCELED
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCRowsetUpdateErrors::Variation_1()
{
    int nResult = TEST_FAIL;
    IConnectionPointContainer *pIConnectionPointContainer = NULL;
    IConnectionPoint *pIConnectionPoint = NULL;
    TCRowsetUpdateNotify notifySink;
    DWORD dwCookie = 0;
    DBBINDING binding = { 1,0,0,0,NULL,NULL,NULL,DBPART_VALUE,0,0,0,0,DBTYPE_I4,0,0 };
    HACCESSOR hAccessor = NULL;
    DBCOUNTITEM ulTemp;
    LONG lTemp;
    HROW hRow = NULL;
    HROW *phRow = &hRow;

    m_pIRowset->RestartPosition(NULL);
    TESTC_(m_pIRowset->GetNextRows(NULL, 0, 1, &ulTemp, &phRow), S_OK);
    TESTC_(m_pIAccessor->CreateAccessor(DBACCESSOR_ROWDATA, 1, &binding, 0, &hAccessor, NULL), S_OK);

    TESTC_(m_pIRowset->QueryInterface(IID_IConnectionPointContainer, (void**)&pIConnectionPointContainer), S_OK);
    TESTC_(pIConnectionPointContainer->FindConnectionPoint(IID_IRowsetNotify, &pIConnectionPoint), S_OK);
    TESTC_(pIConnectionPoint->Advise((IUnknown *)&notifySink, &dwCookie), S_OK);

    lTemp = 1;
    nResult = VerifyMessage(DB_E_CANCELED, m_pIRowsetUpdate->SetData(hRow, hAccessor, (void *)&lTemp));

    // TO DO:  Add your own code here
CLEANUP:
    if (hRow)
        m_pIRowset->ReleaseRows(1, &hRow, NULL, NULL, NULL);
    SAFE_RELEASE_ACCESSOR(m_pIAccessor, hAccessor);
    if (dwCookie)
        pIConnectionPoint->Unadvise(dwCookie);
    SAFE_RELEASE(pIConnectionPoint);
    SAFE_RELEASE(pIConnectionPointContainer);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCRowsetUpdateErrors::Terminate()
{
    // TO DO:  Add your own code here
    SAFE_RELEASE(m_pIAccessor);
    SAFE_RELEASE(m_pIRowsetUpdate);
    SAFE_RELEASE(m_pIRowset);
    ReleaseCommandObject();
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CCommandObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCTransactionLocalErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCTransactionLocalErrors - ITransactionLocal Error Messages
//| Created:  	00/03/13
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCTransactionLocalErrors::Init()
{
    BOOL bResult = FALSE;
    m_pITransactionLocal = NULL;

    // {{ TCW_INIT_BASECLASS_CHECK
    if(CSessionObject::Init())
        // }}
    {
        // TO DO:  Add your own code here
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        if(!VerifyInterface(m_pThisTestModule->m_pIUnknown2, IID_ITransactionLocal, SESSION_INTERFACE, (IUnknown**)&m_pITransactionLocal))
        {
            odtLog <<"ITransactionLocal interface is not supported by the provider\n";
            bResult = TEST_SKIPPED;
            goto CLEANUP;
        }

        bResult = TRUE;

    }
CLEANUP:
    return bResult;
}




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc XACT_E_NOTRANSACTION (8004D00E)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCTransactionLocalErrors::Variation_1()
{
    return VerifyMessage(XACT_E_NOTRANSACTION, m_pITransactionLocal->Abort(NULL, 0, 0));
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc XACT_E_XTIONEXISTS (8004D013)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCTransactionLocalErrors::Variation_2()
{
    int nResult = TEST_FAIL;
    ULONG ulTemp;
    TESTC_(m_pITransactionLocal->StartTransaction(ISOLATIONLEVEL_READCOMMITTED, 0, NULL, &ulTemp), S_OK);
    nResult = VerifyMessage(XACT_E_XTIONEXISTS, m_pITransactionLocal->StartTransaction(ISOLATIONLEVEL_READCOMMITTED, 0, NULL, &ulTemp));
CLEANUP:
    if(m_pITransactionLocal)
        m_pITransactionLocal->Abort(NULL, 0, 0);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc XACT_E_ISOLATIONLEVEL(8004D008)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCTransactionLocalErrors::Variation_3()
{
    int nResult = TEST_FAIL;
    ITransactionJoin *	pITransactionJoin = NULL;

    if( !VerifyInterface(m_pITransactionLocal, IID_ITransactionJoin, SESSION_INTERFACE, (IUnknown**)&pITransactionJoin))
        return TEST_SKIPPED;

    //nResult = VerifyMessage(XACT_E_ISOLATIONLEVEL, pITransactionJoin->JoinTransaction(NULL, ULONG_MAX, 0, NULL));
    nResult = VerifyMessage(XACT_E_NOISORETAIN, pITransactionJoin->JoinTransaction(pITransactionJoin, ISOLATIONLEVEL_READUNCOMMITTED, 1, NULL));

    SAFE_RELEASE(pITransactionJoin);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCTransactionLocalErrors::Terminate()
{
    // TO DO:  Add your own code here
    SAFE_RELEASE(m_pITransactionLocal);
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CSessionObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END




// {{ TCW_TC_PROTOTYPE(TCColumnsInfo)
//*-----------------------------------------------------------------------
//| Test Case:		TCColumnsInfo - IColumnsInfo Error Messages
//| Created:  	00/03/23
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCColumnsInfo::Init()
{
    BOOL bResult = FALSE;
    m_pIColumnsInfo = NULL;

    // {{ TCW_INIT_BASECLASS_CHECK
    if(CCommandObject::Init())
        // }}
    {
        // TO DO:  Add your own code here
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        TESTC(CreateCommandObject() == S_OK);
        TESTC(VerifyInterface(m_pICommand, IID_IColumnsInfo, COMMAND_INTERFACE, (IUnknown**)&m_pIColumnsInfo));
        bResult = TRUE;
    }
CLEANUP:
    return bResult;
}




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_NOCOMMAND (80040E0C)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCColumnsInfo::Variation_1()
{
    DBCOUNTITEM cColumns = 0;
    DBCOLUMNINFO *rgColumnInfo = NULL;
    OLECHAR *pStringBuffer = NULL;

    // No error info for this
    TESTC_(m_pIColumnsInfo->GetColumnInfo(&cColumns, &rgColumnInfo, &pStringBuffer), DB_E_NOCOMMAND);

CLEANUP:
    SAFE_FREE(rgColumnInfo);
    SAFE_FREE(pStringBuffer);

    return TEST_PASS;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCColumnsInfo::Terminate()
{
    // TO DO:  Add your own code here
    SAFE_RELEASE(m_pIColumnsInfo);
    ReleaseCommandObject();
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CCommandObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCColumnsRowsetErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCColumnsRowsetErrors - IColumnsRowset Error Messages
//| Created:  	00/03/23
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCColumnsRowsetErrors::Init()
{
    BOOL bResult = FALSE;
    WCHAR*		pSQLSet = NULL;
    m_pIColumnsRowset = NULL;
    HRESULT hr = E_FAIL;

    // {{ TCW_INIT_BASECLASS_CHECK
    if(CCommandObject::Init())
        // }}
    {
        // TO DO:  Add your own code here
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        SetTable((CTable *)m_pThisTestModule->m_pVoid, DELETETABLE_NO);
        CreateCommandObject();

        // Creating Rowset Object
        TESTC(SUCCEEDED(m_pTable->CreateSQLStmt(SELECT_ALLFROMTBL,NULL,
                                                &pSQLSet, NULL, NULL)));
        hr = ::ExecuteCommand(m_pICommand, pSQLSet, IID_IColumnsRowset, (IUnknown **)&m_pIColumnsRowset);
        if( FAILED(hr) )
        {
            if(E_NOINTERFACE == hr)
                bResult = TEST_SKIPPED;

            goto CLEANUP;
        }
        bResult = TRUE;
    }
CLEANUP:
    PROVIDER_FREE(pSQLSet);
    return bResult;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_BADCOLUMNID (80040E11)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCColumnsRowsetErrors::Variation_1()
{
    IUnknown *pIUnknown = NULL;
    DBID rgOptColumns[1];

    memset(rgOptColumns, 0, sizeof(rgOptColumns));
    rgOptColumns[0].eKind = DBKIND_NAME;
    rgOptColumns[0].uName.pwszName = L"THIS IS AN INVALID COLUMN NAME";

    // No error info for this
    TESTC_(m_pIColumnsRowset->GetColumnsRowset(NULL, 1, rgOptColumns, IID_IUnknown, 0, NULL, &pIUnknown), DB_E_BADCOLUMNID);

CLEANUP:
    SAFE_RELEASE(pIUnknown);
    return TEST_PASS;
}
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCColumnsRowsetErrors::Terminate()
{
    // TO DO:  Add your own code here
    SAFE_RELEASE(m_pIColumnsRowset);
    ReleaseCommandObject();
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CCommandObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCCommandPropertiesErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCCommandPropertiesErrors - ICommandProperties Error Messages
//| Created:  	00/03/29
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCCommandPropertiesErrors::Init()
{
    m_pICommandProperties =  NULL;

    // {{ TCW_INIT_BASECLASS_CHECK
    if(CCommandObject::Init())
        // }}
    {
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        SetTable((CTable *)m_pThisTestModule->m_pVoid, DELETETABLE_NO);
        CreateCommandObject();
        return VerifyInterface(m_pICommand, IID_ICommandProperties, COMMAND_INTERFACE, (IUnknown**)&m_pICommandProperties);
    }

    return FALSE;
}




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_OBJECTOPEN
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCommandPropertiesErrors::Variation_1()
{
    int nResult = TEST_FAIL;
    WCHAR*		pSQLSet = NULL;
    IRowset *pIRowset = NULL;
    DBPROPSET dbpropset;
    DBPROP dbprop;


    TESTC(SUCCEEDED(m_pTable->CreateSQLStmt(SELECT_ALLFROMTBL,
                                            NULL, &pSQLSet, NULL, NULL)));
    TESTC_(::ExecuteCommand(m_pICommand, pSQLSet, IID_IRowset, (IUnknown **)&pIRowset), S_OK);
    dbpropset.cProperties = 1;
    dbpropset.guidPropertySet = DBPROPSET_ROWSET;
    dbpropset.rgProperties = &dbprop;
    memset(&dbprop, 0, sizeof(dbprop));
    dbprop.dwPropertyID = DBPROP_BOOKMARKS;
    dbprop.vValue.vt = VT_BOOL;
    dbprop.vValue.boolVal = TRUE;

    VerifyMessage(DB_E_OBJECTOPEN, m_pICommandProperties->SetProperties(1, &dbpropset));

    nResult = TEST_PASS;

CLEANUP:
    SAFE_RELEASE(pIRowset);
    PROVIDER_FREE(pSQLSet);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCCommandPropertiesErrors::Terminate()
{
    // TO DO:  Add your own code here
    SAFE_RELEASE(m_pICommandProperties);
    ReleaseCommandObject();
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CCommandObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCConvertTypeErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCConvertTypeErrors - IConvertType Error Messages
//| Created:  	00/03/29
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCConvertTypeErrors::Init()
{
    // {{ TCW_INIT_BASECLASS_CHECK
    if(CCommandObject::Init())
        // }}
    {
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        SetTable((CTable *)m_pThisTestModule->m_pVoid, DELETETABLE_NO);
        CreateCommandObject();
        return VerifyInterface(m_pICommand, IID_IConvertType, COMMAND_INTERFACE, (IUnknown**)&m_pIConvertType);
    }
    return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_BADCONVERTFLAG
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCConvertTypeErrors::Variation_1()
{
    return VerifyMessage(DB_E_BADCONVERTFLAG,  m_pIConvertType->CanConvert(DBTYPE_STR, DBTYPE_WSTR, 666));
}
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc DB_E_BADTYPE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCConvertTypeErrors::Variation_2()
{
    return VerifyMessage(DB_E_BADTYPE,  m_pIConvertType->CanConvert(DBTYPE_STR, DBTYPE_WSTR, DBCONVERTFLAGS_FROMVARIANT));
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCConvertTypeErrors::Terminate()
{
    // TO DO:  Add your own code here
    SAFE_RELEASE(m_pIConvertType);
    ReleaseCommandObject();
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CCommandObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCDBCreateCommandErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCDBCreateCommandErrors - IDBCreateCommand Error Messages
//| Created:  	00/03/30
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCDBCreateCommandErrors::Init()
{
    // {{ TCW_INIT_BASECLASS_CHECK
    if(CTestCases::Init())
        // }}
    {
        // TO DO:  Add your own code here
        return  TRUE;
    }
    return FALSE;
}




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_NOAGGREGATION
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDBCreateCommandErrors::Variation_1()
{
    int nResult = TEST_FAIL;
    IDBCreateCommand *pIDBCreateCommand = NULL;
    IUnknown *pIUnknown = NULL;

    TESTC(VerifyInterface(m_pThisTestModule->m_pIUnknown2, IID_IDBCreateCommand, SESSION_INTERFACE, (IUnknown**)&pIDBCreateCommand));
    nResult = ::VerifyMessage(DB_E_NOAGGREGATION, pIDBCreateCommand->CreateCommand(m_pThisTestModule->m_pIUnknown2, IID_IRowset, &pIUnknown), pIDBCreateCommand, IID_IDBCreateCommand);

CLEANUP:
    SAFE_RELEASE(pIUnknown);
    SAFE_RELEASE(pIDBCreateCommand);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCDBCreateCommandErrors::Terminate()
{
    // TO DO:  Add your own code here

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CTestCases::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCDBCreateSessionErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCDBCreateSessionErrors - IDBCreateSession Error Messages
//| Created:  	00/03/30
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCDBCreateSessionErrors::Init()
{
    // {{ TCW_INIT_BASECLASS_CHECK
    if(CTestCases::Init())
        // }}
    {
        // TO DO:  Add your own code here
        return TRUE;
    }
    return FALSE;
}




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_NOAGGREGATION
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDBCreateSessionErrors::Variation_1()
{
    int nResult = TEST_FAIL;
    IDBCreateSession *pIDBCreateSession = NULL;
    IUnknown *pIUnknown = NULL;

    pIDBCreateSession = (IDBCreateSession *)m_pThisTestModule->m_pIUnknown;
    nResult = ::VerifyMessage(DB_E_NOAGGREGATION, pIDBCreateSession->CreateSession(m_pThisTestModule->m_pIUnknown, IID_IRowset, &pIUnknown), pIDBCreateSession, IID_IDBCreateSession);

    SAFE_RELEASE(pIUnknown);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCDBCreateSessionErrors::Terminate()
{
    // TO DO:  Add your own code here

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CTestCases::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCDBInitializeErrors)
//*-----------------------------------------------------------------------
//| Test Case:		TCDBInitializeErrors - IDBInitialize Error Messages
//| Created:  	00/03/30
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCDBInitializeErrors::Init()
{
    // {{ TCW_INIT_BASECLASS_CHECK
    if(CTestCases::Init())
        // }}
    {
        // TO DO:  Add your own code here
        return TRUE;
    }
    return FALSE;
}




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_ALREADYINITIALIZED
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDBInitializeErrors::Variation_1()
{
    IDBInitialize *pIDBInitialize = NULL;

    TESTC(VerifyInterface(m_pThisTestModule->m_pIUnknown, IID_IDBInitialize, DATASOURCE_INTERFACE, (IUnknown**)&pIDBInitialize));

    TESTC_(pIDBInitialize->Initialize(), DB_E_ALREADYINITIALIZED);

CLEANUP:
    SAFE_RELEASE(pIDBInitialize);
    return TEST_PASS;
}
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCDBInitializeErrors::Terminate()
{
    // TO DO:  Add your own code here

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CTestCases::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END



// {{ TCW_TC_PROTOTYPE(TCRowsetIdentityErrors)
//*-----------------------------------------------------------------------
//| Test Case:	TCRowsetIdentityErrors - IRowsetIdentity Error Messages
//| Created:  	00/03/30
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCRowsetIdentityErrors::Init()
{
    // {{ TCW_INIT_BASECLASS_CHECK
    if(CCommandObject::Init())
        // }}
    {
        SetDBSession(m_pThisTestModule->m_pIUnknown2);
        SetTable((CTable *)m_pThisTestModule->m_pVoid, DELETETABLE_NO);
        CreateCommandObject();
        return TRUE;
    }
    return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_BADROWHANDLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCRowsetIdentityErrors::Variation_1()
{
    int nResult = TEST_FAIL;
    WCHAR*		pSQLSet = NULL;
    IRowsetIdentity *pIRowsetIdentity = NULL;
    TESTC(SUCCEEDED(m_pTable->CreateSQLStmt(SELECT_ALLFROMTBL,
                                            NULL, &pSQLSet, NULL, NULL)));
    TESTC_(::ExecuteCommand(m_pICommand, pSQLSet, IID_IRowsetIdentity, (IUnknown **)&pIRowsetIdentity), S_OK);

    nResult = ::VerifyMessage(DB_E_BADROWHANDLE, pIRowsetIdentity->IsSameRow(666, 666), (IUnknown *)pIRowsetIdentity, IID_IRowsetIdentity);

    // TO DO:  Add your own code here
CLEANUP:
    SAFE_RELEASE(pIRowsetIdentity);
    PROVIDER_FREE(pSQLSet);
    return nResult;
}
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TEST_PASS or TEST_FAIL
//
BOOL TCRowsetIdentityErrors::Terminate()
{
    // TO DO:  Add your own code here
    ReleaseCommandObject();
    ReleaseDBSession();

// {{ TCW_TERM_BASECLASS_CHECK2
    return(CCommandObject::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END

