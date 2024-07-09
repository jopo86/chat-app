#pragma once

namespace Onyx { enum class Key; };

namespace ck
{
    extern char all[95];

    Onyx::Key ctok(char c);
    char ktoc(Onyx::Key key);

    char shift(char c);
    char unshift(char c);
}
