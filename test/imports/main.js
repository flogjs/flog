import assert from "../assert.js";
import a from "./a.js";
import b from "a.js";

assert(a === "a");
assert(b === "a");
