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

my @DOC_TAGS = qw(
  legacy-0.8.0
  legacy-0.9.0
  legacy-0.10.0
  legacy-0.11.0
  legacy-1.0.0
  legacy-1.0.1
  legacy-1.0.2
  legacy-1.0.3
  legacy-1.0.4
  legacy-1.0.5
  legacy-1.0.6
  legacy-1.0.7
  legacy-1.1.0
  legacy-1.1.1
  legacy-1.1.2
  r3.0.0
  r3.0.1
  r3.0.2
  r3.0.3
  r3.1.0
);

sub main {
    for my $ref ( @DOC_TAGS ) {
        _try_run("etc/generate-apidocs-from-tag.pl", $ref);
    }
}

main();

