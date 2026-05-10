export function X<T>(image: vec4<T> | vec3<T> | vec2<T>) {
  return image[0];
}

export function Y<T>(image: vec4<T> | vec3<T> | vec2<T>) {
  return image[1];
}

export function Z<T>(image: vec4<T> | vec3<T>) {
  return image[2];
}


export function map(I: image2D, fn: (v: float) => float): image2D {
  return I.map((row) => row.map((val) => fn(val)));
}


export function transform(
  input1: image2D,
  input2: image2D,
  output: image2D,
  fn: (a: float, b: float) => float
): void {
  for (let i = 0; i < output.length; ++i) {
    for (let j = 0; j < output[i].length; ++j) {
      output[i][j] = fn(input1[i][j], input2[i][j]);
    }
  }
}
