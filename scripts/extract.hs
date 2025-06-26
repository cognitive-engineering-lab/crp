#!/usr/bin/env runghc
module Main where

import System.Directory (createDirectoryIfMissing)
import System.Environment (getArgs)
import System.FilePath (dropExtension, splitDirectories, (</>))
import System.IO (readFile)

main :: IO ()
main = do
    [path] <- getArgs
    contents <- readFile path
    let contentLines = lines contents
    let listings = convert contentLines
    let numbered = zip [0 ..] listings
    flip traverse numbered $ \(i, listing) -> do
        let dirName = "listings" </> "cc" </> (foldl1 (</>) (drop 1 (splitDirectories (dropExtension path)))) <> "-" <> show i
        createDirectoryIfMissing True dirName
        let outputName = dirName </> "example.cc"
        writeFile outputName listing
    pure ()

convert :: [String] -> [String]
convert content =
    if null content
        then []
        else case extract content of
            Just (listing, remainder) -> listing : convert remainder
            Nothing -> []

extract :: [String] -> Maybe (String, [String])
extract content =
    let x = dropWhile (not . (== "```cpp")) content
     in if null x
            then Nothing
            else
                let (listing, rest) = break (== "```") $ drop 1 x
                 in Just (unlines listing, drop 1 rest)
