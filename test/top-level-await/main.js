import assert from "../assert.js";

assert((await (async () => 2)()) === 2);
// implicitly converting a normal function to a promise isn't supported yet in
// tla
//assert((await (() => 2)()) === 2);
