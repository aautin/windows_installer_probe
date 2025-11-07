# Windows Installer Probe

A Qt-based application that replicates the Windows Installer UI GUI/CLI by integrating with the Microsoft Installer (MSI) API.

## Overview

This application provides a custom installation UI that works with an EXE bootstrappers, automatic extraction
and installation of embedded MSI packages by intercepting MSI installation callbacks.

## Supported Formats

✅ **Fully Supported:**
- MSI-based bootstrappers with extraction support (common setup.exe files)

⚠️ **Limited Support:**
- Some self-extracting archives (depends on format)

❌ **Not Supported:**
- Native InstallShield installers (non-MSI)
- NSIS installers
- Inno Setup installers

## Prerequisites

**Note**: This project uses internal SBG Conan packages and may not be buildable outside the SBG network.

Internal dependencies:
- `conan sbg:install` command
- `sbg-cmake-tools` (from `@sbg/stable` remote)

## Flowchart

```mermaid
flowchart TD
    A["Input: setup.exe"] --> B["Extraire le contenu du .exe"]
    B --> C{"MSI trouvé?"}
    C -->|"Oui"| D["Identifier les dépendances<br/>(CAB, DLL, etc.)"]
    C -->|"Non"| E["Erreur: MSI non trouvé"]
    D --> F["Copier MSI + dépendances<br/>dans répertoire temporaire"]
    F --> G["Installer via MsiSetExternalUI"]
    G --> H["MsiInstallProduct avec<br/>chemin du MSI extrait"]
    H --> I["Nettoyage fichiers temporaires"]
```