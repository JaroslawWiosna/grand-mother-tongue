# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.8.1] - 2020-12-16
### Fixed
- (#59) Improve performance of `Hash_Map::contains(Key key)`

## [0.8.0] - 2020-12-12
### Added
- (#48) Subcomand `search` that outputs Wikidata ID

### Change
- (#52) Subcommand `do` that executes the 'as of yet' program
  - This change require to use subcommands:
    - `do` or
    - `search`

## [0.7.1] - 2020-12-07
### Fixed
- (#32) Unhardcode `--blood-plot` output file

## [0.7.0] - 2020-12-06
### Added
- (#39) Ping to check the Internet connection before REST API call

## [0.6.1] - 2020-12-05
### Fixed
- (#35) unlimited `--ahnentafel` (for more than first 7 people)

## [0.6.0] - 2020-12-04
### Added
- Add `-v` for verbosity
- Add `--ahnentafel` for the first 7 people

## [0.5.0] - 2020-11-28
### Added
- (#15) `--blood-plot`

## [0.4.1] - 2020-11-28
### Fixed
- free hashmap inside Blood in `PhoneBook::print_origin_by_blood(PersonID)`

## [0.4.0] - 2020-11-27
### Added
- (#4) Find the year of birth and death

## [0.3.1] - 2020-11-27
### Fixed
- (#14) `--dump-db <path>` dumps to `<path>` instead to the `dumped.txt`
- (#6) Crash if `<path>` does not exist in `--db <path>`

## [0.3.0] - 2020-11-26
### Added
- (#23) Support for `--max-generations` in argv

## [0.2.0] - 2020-11-26
### Added
- Project description in README
- (#13) Implement `--version`

## [0.1.0] - 2020-11-26
### Added
- initial proof of concept

### Changed

### Deprecated

### Removed

### Fixed

### Security
