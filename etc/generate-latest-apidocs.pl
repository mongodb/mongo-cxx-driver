#!/usr/bin/env perl
use v5.14;
use strict;
use warnings;
use utf8;
use open qw/:std :utf8/;

# system() wrapper to die with an error if a command fails.
sub _try_run {
    my @command = @_;
    say "> Running: @command";
    system(@command);
    die "Error running '@command" if $?;
}

my $LATEST_DOC_TAG = "r3.11.1";

sub main {
    _try_run("etc/generate-apidocs-from-tag.pl", $LATEST_DOC_TAG);
}

main();
