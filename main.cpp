#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QProcess>
#include <QTemporaryDir>
#include <QString>
#include <QMessageBox>

#include <windows.h>
#include <msi.h>

static int _stdcall UIHandler(LPVOID pvContext, UINT iMessageType, LPCSTR szMessage)
{
	switch ((INSTALLMESSAGE)(0xFF000000 & (UINT)iMessageType)) {

	case INSTALLMESSAGE_PROGRESS:
		qDebug() << "Progress:" << QString::fromUtf8(szMessage);
		break;
	case INSTALLMESSAGE_ERROR:
		qDebug() << "Error:" << QString::fromUtf8(szMessage);
		break;
	/*case INSTALLMESSAGE_INFO:
		qDebug() << "Info:" << QString::fromUtf8(szMessage);
		break;*/
	case INSTALLMESSAGE_FATALEXIT:
		qDebug() << "Fatalexit:" << QString::fromUtf8(szMessage);
		break;
	case INSTALLMESSAGE_WARNING:
		qDebug() << "Warning:" << QString::fromUtf8(szMessage);
		break;
	case INSTALLMESSAGE_USER:
		qDebug() << "User:" << QString::fromUtf8(szMessage);
		break;
	}
	return IDOK;
}

static void install(const QStringList& msiFiles, const QString& installPath) {
	QString appdir = QString("APPDIR=\"%1\"").arg(QDir::toNativeSeparators(QDir(installPath).absolutePath()));
	// Implementation of the installation logic goes here
	qDebug() << "Installing :" << msiFiles;
	qDebug() << "Msi arg :" << appdir;

	MsiSetInternalUI(INSTALLUILEVEL_NONE, nullptr);
	MsiSetExternalUIA(UIHandler,
		INSTALLLOGMODE_PROGRESS | INSTALLLOGMODE_FATALEXIT | INSTALLLOGMODE_ERROR |
		INSTALLLOGMODE_WARNING | INSTALLLOGMODE_USER | INSTALLLOGMODE_INFO |
		INSTALLLOGMODE_FILESINUSE | INSTALLLOGMODE_RESOLVESOURCE |
		INSTALLLOGMODE_OUTOFDISKSPACE | INSTALLLOGMODE_ACTIONSTART |
		INSTALLLOGMODE_ACTIONDATA | INSTALLLOGMODE_COMMONDATA,
		nullptr);

	for (const QString& msiFile : msiFiles) {
		
		UINT result = MsiInstallProductW(
			reinterpret_cast<LPCWSTR>(msiFile.utf16()),
			reinterpret_cast<LPCWSTR>(appdir.utf16())
		);
		if (result != ERROR_SUCCESS) {
			qCritical() << "Installation of" << msiFile << "failed with error code:" << result;
		} else {
			qDebug() << "Installation of" << msiFile << "completed successfully.";
		}
	}
}

int main(int argc, char* argv[]) {
	QCoreApplication app(argc, argv);
	
	//
	// Define parser
	//
	QCommandLineParser parser;
	parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsCompactedShortOptions);
	parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsOptions);
	parser.addHelpOption();

	parser.addPositionalArgument("bootstrapper",
		"The bootstrapper file to process");
	parser.addOption(QCommandLineOption({ "install-path", "i" },
		"The installation path; the directory will be created if it doesn't already exist.", "install-path",
		"install"));
	
	//
	// Parsing of the command line arguments
	//
	if (!parser.parse(QCoreApplication::arguments())) {
		qCritical() <<"Error:" << parser.errorText();
		parser.showHelp(1);
	}

	auto arguments = parser.positionalArguments();
	if (arguments.size() !=1) {
		qCritical() << "Error: exactly one bootstrapper file must be specified";
		parser.showHelp(1);
	}

	//
	// Bootstrapper file check
	//
	QFileInfo bootstrapperFileInfo(arguments.at(0));
	if (!bootstrapperFileInfo.isExecutable()) {
		qCritical() << "Error:" << arguments.at(0) << "is not executable.";
		return EXIT_FAILURE;
	}

	//
	// Boostrapper extraction
	//
	QTemporaryDir extractionDirectory;
	QString extractionPath = QDir::toNativeSeparators(extractionDirectory.path());

	QString script = "& { Start-Process -FilePath $args[0] -ArgumentList $args[1],$args[2],$args[3] -Wait }";
	QStringList psArgs;
	psArgs 
		<< "-Command" << script << "--" << bootstrapperFileInfo.absoluteFilePath()
		<< "/exenoui" << "/extract" << extractionPath;

	QProcess extractor;
	extractor.start("powershell.exe", psArgs);
	if (!extractor.waitForFinished()) {
	    qCritical() << "Extraction timed out. process state:" << extractor.state() << "error:" << extractor.error();
	    qDebug() << extractor.readAll();
	    return EXIT_FAILURE;
	}
	if (extractor.exitCode() != 0) {
	    qCritical() << "Extraction failed, exit code:" << extractor.exitCode();
	    return EXIT_FAILURE;
	}

	//
	// Installation path check
	//
	QDir installDir(parser.value("install-path"));
	if (!installDir.exists()) {
		if (!installDir.mkpath(".")) {
			qCritical() << "Error: could not create installation path" << installDir.absolutePath();
			return EXIT_FAILURE;
		}
	}

	//
	// Installation process
	//
	auto msiFiles = QDir(extractionPath).entryList(QStringList() << "*.msi", QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
	for (QString& msiFile : msiFiles) msiFile = QDir::toNativeSeparators(QDir(extractionPath).absoluteFilePath(msiFile));
	install(msiFiles, parser.value("install-path"));

	return EXIT_SUCCESS;
}
