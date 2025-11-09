#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QProcess>
#include <QTemporaryDir>
#include <QString>

#include <windows.h>
#include <msi.h>

static void install(const QStringList& msiFiles, const QString& installPath) {
	// Implementation of the installation logic goes here
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
	// Installation process
	//
	install(QDir(extractionPath).entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot), parser.value("install-path"));

	return EXIT_SUCCESS;
}
