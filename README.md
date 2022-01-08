# Blockgame

Blockgame is a fork of [MultiMC] which enables Minecraft for
unsupported (mainly UNIX-like) operating systems.

If you want to play Minecraft on Windows, Linux/amd64, macOS, please
use [MultiMC] instead of Blockgame.

## Background

Blockgame was made out of necessity due to MultiMC branding guidelines
which disallow the usage of the MultiMC brand for forks and custom
builds.

As Blockgame is originally the OpenBSD port of MultiMC, a separate
fork has to be made which strips every single instances of MultiMC
on top of code that is required to add Minecraft support on OpenBSD.

In addition, Blockgame removed the bundle system and targets
POSIX/FHS-style system installations which allows packages/ports to be
made by separate package/port maintainers.

This is done out of necessity mainly because OpenBSD does not have any
ABI guarantees and supports a lot more nuanced architectures which the
maintainer does not have access to.

While Blockgame was made mainly to allow OpenBSD users to enjoy
Minecraft, the project is open to other operating systems /
architectures, assuming there are people who are willing to maintain
it.

## Organization

Currently, the project is maintained solely by [Yuki] who is also
responsible for the OpenBSD port.

The project only follows stable releases of MultiMC and Blockgame's
patches will be backported for newer releases as they arrive.

This project is done voluntarily and new releases should not be
expected to arrive as soon as MultiMC made a new release. However, any
help to backport patches is appreciated.

## Contact

For any inquiries, patches, questions, and so-on, please contact
[Yuki]. Their contact information is available on their
[website][Yuki].

We accept pull requests but emailing patches is preferred due to how
the project is organized.

Please do not contact the [MultiMC] project nor their contributors if
you're using Blockgame.

[MultiMC]: https://multimc.org
[Yuki]: https://yukiisbo.red
