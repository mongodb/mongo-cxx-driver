## Contributing to the MongoDB Legacy C++ Driver Project

Pull requests are always welcome, and the MongoDB dev team appreciates any help the community can
give to help make MongoDB better.

For any particular improvement you want to make, you can begin a discussion on the
[MongoDB Developers Forum][dev-forum]. This is the best place discuss your proposed improvement (and its
implementation) with the core development team.

[dev-forum]: https://groups.google.com/forum/?fromgroups#!forum/mongodb-dev "MongoDB Developers Forum"


## Getting Started

- Create a [MongoDB JIRA account][jira-account].
- Create a [Github account][github-account].
- Fork the repository on Github at https://github.com/mongodb/mongo-cxx-driver.
- Check out the 'legacy' branch 'git checkout legacy'

[jira-account]: https://jira.mongodb.org/secure/Signup!default.jspa "MongoDB JIRA Signup"
[github-account]: https://github.com/signup/free "Githup Signup"


## JIRA Tickets

All commits to the C++ driver  repository must reference an issue in the [CXX project][cxx-project]
of the MongoDB JIRA. Before creating any new tickets, please search the existing backlog for any open
tickets that represent your change request. If there is not one, then you should create a new
ticket. Tickets specific to the legacy driver should be filed the label [legacy-cxx][legacy-cxx].

For bugs, please clearly describe the issue you are resolving, including the platforms on which
the issue is present and clear steps to reproduce.

For improvements or feature requests, be sure to explain the goal or use case and the approach
your solution will take.

[cxx-project]: https://jira.mongodb.org/browse/CXX
[legacy-cxx]: https://jira.mongodb.org/browse/CXX-69?jql=labels%20%3D%20legacy-cxx


## The Life Cycle of a Pull Request

Here's what happens when you submit a pull request:

- The MongoDB engineering team will review your pull request to make sure you have included a
  CXX ticket in your request and signed the contributor agreement.
- You should receive a response from one of our engineers with additional questions about your
  contributions.
- If your pull request matches a ticket and is aligned with the Server Roadmap, it will get
  triaged and reviewed by the C++ driver and Kernel teams.
- Pull requests that have been reviewed and approved will be signed off and merged into a
  development branch and the associated JIRA CXX issue will be resolved with an expected
  fixVersion.


## Style Guide

All commits to the legacy branch must follow the [kernel development rules][kernel-dev-rules].

In particular, all code must follow the MongoDB [kernel code style guidelines][kernel-style-guide].
For anything not covered in this document you should default to the [Google CPP Style Guide][google-style-guide].

Your commit message should also be prefaced with the relevant JIRA ticket, e.g. "CXX-XXX Fixed a bug in aggregation".

[kernel-dev-rules]: http://dochub.mongodb.org/core/kernelcodedevelopmentrules
[kernel-style-guide]: http://dochub.mongodb.org/core/kernelcodestyle
[google-style-guide]: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml


## Testing

Every non-trivial change to the code base should be accompanied by a relevant addition to or
modification of the test suite.  If you don't believe this is necessary, please add an explanation
in the JIRA ticket why no such changes are either needed or possible.

All changes must also pass the full test suite (including your test additions/changes) on your
local machine before you open a pull request.


## Contributor Agreement

A patch will only be considered for merging into the upstream codebase after you have signed the
[contributor agreement][contributor-agreement].

[contributor-agreement]: http://www.mongodb.com/contributor "MongoDB Contributor Agreement"
