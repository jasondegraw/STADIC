Developer Workflow
==================

STADIC was originally developed using a minimal number of branches (master plus branches for big changes).
This is easy with a couple of developers with no releases to worry about, but once releases need to be
tracked it is worthwile to adopt a procedure. To that end, we're adopting Vincent Driessen's branching
model (described fully here: http://nvie.com/posts/a-successful-git-branching-model/).

This workflow makes use of two infinite-lifetime branches:

* master - code at a production-ready state
* develop - code that reflects the current state of development

The original does a very good job of describing the workflow, so it is worth the time to read the document.
Rather than rehash that, this document will describe what steps to take to use the workflow with our GitHub
repo.

Add a New Feature or Fix a Bug
------------------------------

Each new feature or bug fix should be associated with a GitHub issue. If there isn't an issue, submit one and
get yourself assigned to it. Then:

1. Create a new branch from develop. Make sure that the branch is of develop and not of master and give the
branch a name like "issue-68", where 68 is the issue number. If your "u", "e", and "-" keys are defective,
then "iss68" is OK. It is also OK to add the name of the feature or a description to the branch name (e.g. 
"issue-68-fix-that-thing")

        $> git checkout develop
        $> git branch issue-68
        $> git checkout issue-68
    
2. Write code.
3. Create a pull request. Once your code has been reviewed and merged, delete the branch.


