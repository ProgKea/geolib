{
  description = "";

  outputs = { self, nixpkgs }:
  let
    system = "x86_64-linux";
    pkgs = import nixpkgs {inherit system;};
  in
  {
    devShells.${system}.default = with pkgs; mkShell {
      buildInputs = [
        raylib
        clang
        pkg-config
      ];
    };
  };
}
