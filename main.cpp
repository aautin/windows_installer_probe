#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>

static void install(const QString& bootstrapper, const QString& installPath) {
	qDebug() << "Installing bootstrapper:" << bootstrapper << "to path:" << installPath;
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
	if (arguments.size() != 1) {
		qCritical() << "Error: exactly one bootstrapper file must be specified";
		parser.showHelp(1);
	}

	//
	// Installation process
	//
	install(arguments.at(0), parser.value("install-path"));

	return EXIT_SUCCESS;
}
