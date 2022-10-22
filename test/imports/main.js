import assert from "../assert.js";
import a from "./a.js";
import b from "a.js";

assert(a === "1");
assert(b === "1");
